#include "string.h"
#include "assert.h"
#include "common.h"

/*
 * use putchar_func to print a string
 *   @putchar_func is a function pointer to print a character
 *   @format is the control format string (e.g. "%d + %d = %d")
 *   @data is the address of the first variable argument
 * please implement it.
 */
int
vfprintf(void (*putchar_func)(char), const char *format, void **data) {
	const char *s;
	int charnum = 0; //记录输出字符数量
	/*for (s = "please implement me\n"; *s; s ++) {
		putchar_func(*s);
	}*/
	for(s = format; *s;s ++){
		if(*s == '%' && *(s+1) == '%'){//首先处理百分号转义
			putchar_func(*++s);
			charnum++;
		}
		else if(*s == '%' && *(s+1) == 'd'){
			s++;//跳过后面的'd'
			if(data)//以防万一data为空虽然好像没什么用因为data是void类型的没法预料
			{
				int *da = (int *)*(data++);
				//if(da){//以防万一da为空
					char *str = itoa((int)da,10);//偷懒了:)
					for(;*str;str++){//因为itoa不会返回NULL所以就直接输出
						putchar_func(*str);
						charnum++;
					}
				//}
			}

		}
		else if(*s == '%' && *(s+1) == 'x'){
			s++;//跳过后面的'x'
			if(data){
				int *da = (int *)*(data++);
				//if(da){
					char *str = itoa((int)da,16);//同样偷懒了,不用白不用:)
					for(;*str;str++){//因为itoa不会返回NULL所以直接输出
						putchar_func(*str);
						charnum++;
					}
				//}
			}
		}
		else if(*s == '%' && *(s+1) == 's'){
			s++;//跳过后面的's'
			if(data){
				char *da = (char *)*(data++);
				if(da){
					for(;*da;da++){
						putchar_func(*da);
						charnum++;
					}
				}

			}
		}
		else if(*s == '%' && *(s+1) == 'c'){
			s++;//跳过后面的'c'
			if(data){
				char da = '\0'; 
				da = (char)(int)*(data++);//貌似要先转成int才行
				if(da){
					putchar_func(da);
					charnum++;
				}
			}
		}		
		else{
			putchar_func(*s);
			charnum++;
		}
	}
	return charnum;
}

