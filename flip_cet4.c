#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"
#include "file_read.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <SDL/SDL_rotozoom.h>
SDL_Event event;
TTF_Font* font;
SDL_Surface* screen = NULL;
SDL_Surface* message = NULL;
SDL_Surface* message_translate = NULL;

Mix_Music* music = NULL;
//旋转参数
int rotate = 0;
int quit = 0;
const char file_name[] = "cet4.txt";
FILE *fn;
SDL_Color color = {180,0,0};
//单词显示的位置
struct word_position{
    int x;
    int y;
}word_p = {40,70},trans_p = {40,94};


char* itoa(int num,char* str,int radix)
{
    char index[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";//索引表
    unsigned unum;//存放要转换的整数的绝对值,转换的整数可能是负数
    int i=0,j,k;//i用来指示设置字符串相应位，转换之后i其实就是字符串的长度；转换后顺序是逆序的，有正负的情况，k用来指示调整顺序的开始位置;j用来指示调整顺序时的交换。
 
    //获取要转换的整数的绝对值
    if(radix==10&&num<0)//要转换成十进制数并且是负数
    {
        unum=(unsigned)-num;//将num的绝对值赋给unum
        str[i++]='-';//在字符串最前面设置为'-'号，并且索引加1
    }
    else unum=(unsigned)num;//若是num为正，直接赋值给unum
 
    //转换部分，注意转换后是逆序的
    do
    {
        str[i++]=index[unum%(unsigned)radix];//取unum的最后一位，并设置为str对应位，指示索引加1
        unum/=radix;//unum去掉最后一位
 
    }while(unum);//直至unum为0退出循环
 
    str[i]='\0';//在字符串最后添加'\0'字符，c语言字符串以'\0'结束。
 
    //将顺序调整过来
    if(str[0]=='-') k=1;//如果是负数，符号不用调整，从符号后面开始调整
    else k=0;//不是负数，全部都要调整
 
    char temp;//临时变量，交换两个值时用到
    for(j=k;j<=(i-1)/2;j++)//头尾一一对称交换，i其实就是字符串的长度，索引最大值比长度少1
    {
        temp=str[j];//头部赋值给临时变量
        str[j]=str[i-1+k-j];//尾部赋值给头部
        str[i-1+k-j]=temp;//将临时变量的值(其实就是之前的头部值)赋给尾部
    }
 
    return str;//返回转换后的字符串
 
}

int init()
{
    //打开单词
    fn = fopen(file_name,"r");
    if(fn == NULL)
    {
        return -1;
    }
    
	//初始化所有SDL子系统
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        return -1;    
    }
    //初始化手柄按钮
    int joystick_count = SDL_NumJoysticks();
	if(joystick_count > 0)
	{
		for(int i=0; i<joystick_count; i++) SDL_JoystickOpen(i);
		SDL_JoystickEventState(SDL_ENABLE);
	}
    
    //设置窗口
    screen = SDL_SetVideoMode( 320, 240,32, SDL_SWSURFACE );
    //如果设置窗口出错
    if( screen == NULL )
    {
        return -1;    
    }
    
    //初始化SDL_ttf
    if( TTF_Init() == -1 )
    {
        return -1;    
    }
    //设置字体
    font = TTF_OpenFont("Fiery_Turk.ttf",24);
    
    //初始化SDL_mixer
    if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 )
    {
        return -1;    
    }
    //设置窗口标题
    SDL_WM_SetCaption( "flip_cet4", NULL );
    
    //如果所有初始化都成功
    return 1;
}

//加载并播放mp3
int load_and_play_mp3(char* name)
    {
        music = Mix_LoadMUS(name);
        if(music == NULL)
        {
            return -1;
        }
        //如果没有音乐在播放
        //if( Mix_PlayingMusic() == 0 )
        //{
            //播放音乐
            if( Mix_PlayMusic( music, 1 ) == -1 )
            {
				printf("***err:play_err_\n");
                return 1;
            }    
        //}
    }

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip)
{
    //用于保存坐标
    SDL_Rect offset;

    //获得坐标
    offset.x = x;
    offset.y = y;

    //Blit操作
    SDL_BlitSurface( source, clip, destination, &offset );
}

//循环
int fmn()
{
	//单词和翻译
	char words_and_trans[100];
	//此指针指向翻译部分
	char* tmp_char_p;
	//单词
	char words[20];
	//文件名
	char mp3_file_name[30];
	//随机数
	int r;
	//分隔符
	char sep = '*';
	time_t t;
	char head_file[] = "./mp3/";
    while(quit == 0)
    {
        while(SDL_PollEvent(&event))
        {
			
            if(event.type == SDL_QUIT || (event.key.keysym.sym == 27 && event.key.type == SDL_KEYDOWN))
            {
                quit = 1;
            }
			//下一个单词
            else if ((event.key.keysym.sym == 8 || event.key.keysym.sym == 9 || event.key.keysym.sym == 275 || event.key.keysym.sym == 306) && event.key.type == SDL_KEYDOWN)
            {
				//停止播放
				Mix_HaltMusic();
				//掷随机数
				srand((unsigned) time(&t));
				r = rand() % prt_f_words.n;
				fseek(fn,prt_f_words.prt[r],SEEK_SET);
				fgets(words_and_trans,100,fn);
				//printf("words_and_trans:%s\n",words_and_trans);
				//处理words后可能存在的\r\n
				int i=0;
				while(1)
				{
					if(words_and_trans[i] == '\r' || words_and_trans[i] == '\n' || words_and_trans[i] == '\0')
						{
							printf("changeWords:%c\n",words_and_trans[i]);
							words_and_trans[i] = '\0';
							break;	
						}
					i++;
				}
				
				tmp_char_p = strchr(words_and_trans,sep);
				//printf("tmp_char_p:%s\n",tmp_char_p);
				memmove(words,words_and_trans,tmp_char_p - words_and_trans);
				words[tmp_char_p - words_and_trans] = '\0';
				//printf("words:%s\n",words);
				memmove(mp3_file_name,head_file,sizeof(head_file));
				memmove(mp3_file_name+sizeof(head_file) - 1,words_and_trans,tmp_char_p - words_and_trans);
				memmove(mp3_file_name+sizeof(head_file) - 1 + (tmp_char_p - words_and_trans),".mp3",5);
				//printf("mp3_file_name:%s\n",mp3_file_name);
                if(load_and_play_mp3(mp3_file_name) == -1)
                {
					printf("***err:%s,load_failure\n",mp3_file_name);;//返回-1,打开失败
                }
                
                //fscanf(fn, "%s", buff);
                message = TTF_RenderUTF8_Blended(font,words,color);
				
                message_translate = TTF_RenderUTF8_Blended(font,tmp_char_p+1,color);
				//清空屏幕
                SDL_FillRect( screen, &(screen->clip_rect), SDL_MapRGB( screen->format, 0xFF, 0xFF, 0xFF ) );
                if(event.key.keysym.sym == 9)
				{
					rotate = 90;
					word_p.x = 40;
					word_p.y = 70;
					trans_p.x = 64;
					trans_p.y = 0;
				}else if(event.key.keysym.sym == 8)
				{
					rotate = 270;
					word_p.x = 320 - 40;
					word_p.y = 70;
					trans_p.x = 320 - 64;
					trans_p.y = 0;
				}
				else
				{
					rotate = 0;
					word_p.x = 40;
					word_p.y = 70;
					trans_p.x = 40;
					trans_p.y = 94;
				}
				message = rotozoomSurface(message,rotate,1,0);
				message_translate = rotozoomSurface(message_translate,rotate,1,0);

	            apply_surface(word_p.x,word_p.y,message,screen,NULL);
	            apply_surface(trans_p.x,trans_p.y,message_translate,screen,NULL);
	            
	            SDL_Flip(screen);
	            //printf("^^^^^^^^^^^^^^^^^\n");
            }
			//播放音乐
            else if (event.key.keysym.sym == 304 && event.key.type == SDL_KEYDOWN)
            {
				if(load_and_play_mp3(mp3_file_name) == -1)
                {
					printf("***err:%s,load_failure\n",mp3_file_name);;//返回-1,打开失败
                }
            }
            //为了测试案件码
            //~ else if(event.key.type == SDL_KEYDOWN)
            //~ {
                //~ itoa(event.key.keysym.sym,words,10);
                //~ message_translate = TTF_RenderUTF8_Blended(font,words,color);
                //~ //清空屏幕
                //~ SDL_FillRect( screen, &(screen->clip_rect), SDL_MapRGB( screen->format, 0xFF, 0xFF, 0xFF ) );
	            //~ apply_surface(word_p.x,word_p.y + 24,message_translate,screen,NULL);
	            
	            //~ SDL_Flip(screen);
	            //~ //SDL_Delay(1000);
	            //~ //return 0;
            //~ }
        }
    }
}

int main(int argc,char* args[])
{
	printf("start:init()\n");
	init();
	char numjoy[20];
    //初始化单词
    if (file_read_lines(fn))
        return -1;
    itoa(prt_f_words.n,numjoy,10);
	message = TTF_RenderUTF8_Blended(font,numjoy,color);
	message = rotozoomSurface(message,rotate,1,0);
	apply_surface(word_p.x,word_p.y,message,screen,NULL);
	SDL_Flip(screen);
	printf("start:fmn()\n");
	fmn();
	//SDL_Delay(4000);
	//释放音乐
	Mix_FreeMusic(music);
	//退出SDL_mixer
	Mix_CloseAudio();
	//关闭文件
	fclose(fn);
	//释放内存
	file_read_free();
	//释放文字
	SDL_FreeSurface(message);
	TTF_CloseFont(font);
	TTF_Quit();
	SDL_Quit();

	return 0;
}
