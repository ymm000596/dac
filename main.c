#include <alsa/asoundlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>
#include <time.h>
#include "ltpro.h"

unsigned microseconds(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec*1000000 + ts.tv_nsec/1000;
}

unsigned int get_lines(FILE* fp)
{
	char c;
	unsigned int lines = 0;

	fseek(fp,0,SEEK_SET);
	while(fread(&c,1,1,fp))
	{
		if(c == '\n')
		{
			lines+=1;
		}
	}
	return lines;
}

int get_width()
{
	int width;
	char buffer[80] = {0};
	FILE *fp;
	fp=popen("stty size | cut -d\" \" -f2","r");
	fgets(buffer,sizeof(buffer),fp);
	pclose(fp);
	width = atoi(buffer);
	return width;
}

int main(int argc,char* argv[])
{
	unsigned int i;
	int rc;

	if(argc < 5)
	{
		printf("usage:\n%s fs channels data.txt loops\n",argv[0]);
		return -1;
	}
	
    unsigned int fs = atoi(argv[1]); 
	if(fs < 1)
	{
		printf(" fs=%dHz error\n",fs);
		return -2;
	}
    unsigned int ch = atoi(argv[2]);
	if(ch < 1)
	{
		printf(" ch=%d error\n",ch);
		return -3;
	}

	FILE *fp = fopen(argv[3],"r");
	if(NULL == fp)
	{
		printf("open %s failed\n",argv[3]);
		return -4;
	}
	unsigned int loops = atoi(argv[4]);
	
	unsigned int lines = get_lines(fp);

	short* pArray = (short*)malloc(lines*ch*sizeof(short));	
	if(NULL == pArray)
	{
		printf("lines=%d,malloc error\n",lines);
		return -5;
	}
	
	fseek(fp,0,SEEK_SET);
	if(ch < 2)
	{
		for(i=0;i<lines;i++)
		{
			fscanf(fp,"%d",&pArray[i*ch]);
		}
	}
	else
	{
		for(i=0;i<lines;i++)
		{
			fscanf(fp,"%d,%d",&pArray[i*ch],&pArray[i*ch+1]);
		}
	}	
	fclose(fp);

    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    snd_pcm_uframes_t frames = lines;

    rc = snd_pcm_open(&handle,"default",SND_PCM_STREAM_PLAYBACK,0);
    if(rc < 0)
    {
        printf("unable to open pcm device:%s\n",snd_strerror(rc));
        goto EXIT;
    }

    snd_pcm_hw_params_malloc(&params);
    snd_pcm_hw_params_any(handle,params);
    snd_pcm_hw_params_set_access(handle,params,SND_PCM_ACCESS_RW_INTERLEAVED);
    //snd_pcm_hw_params_set_format(handle,params,SND_PCM_FORMAT_FLOAT);
    snd_pcm_hw_params_set_format(handle,params,SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(handle,params,ch);
    snd_pcm_hw_params_set_rate_near(handle,params,&fs,0);

    //snd_pcm_hw_params_set_period_size_near(handle,params,&frames,&dir);

    rc = snd_pcm_hw_params(handle,params);
    if(rc < 0)
    {
        printf(" unable to set hw params:%s\n",snd_strerror(rc));
        goto EXIT;
    }
    
    snd_pcm_hw_params_free(params);
    
    rc= snd_pcm_prepare(handle);
    if(rc < 0)
    {
        printf(" unable to prepare audio to use:%s\n",snd_strerror(rc));
        goto EXIT;
    }
//	printf("Play\n");
	progress_t bar;
    progress_init(&bar, "",get_width()-7, PROGRESS_BGC_STYLE);
    for(i=0;i<loops;i++)
    {
        if(-EPIPE == snd_pcm_writei(handle,pArray,frames))
        {
            printf("underrun\n");
            rc= snd_pcm_prepare(handle);
            if(rc < 0)
            {
                printf(" unable to prepare audio to use:%s\n",snd_strerror(rc));
            }
        }
//		printf("%d / %d\n",i+1,loops);
		progress_show(&bar,(float)(i+1.0f)/(float)loops);
   }
	printf("\n");
	progress_destroy(&bar);
EXIT:
    snd_pcm_close(handle);
    free(pArray);
    exit(0);
}
