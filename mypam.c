#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
//#include <security/pam_appl.h>
//#include <security/pam_modules.h>
//#include <security/pam_ext.h>
void getTime(char str[],int *y,int *m,int *d){ //获取系统时间
    time_t now;
    struct tm *tm;
    now = time(0);			//获得当前系统时间
    if ((tm = localtime (&now)) == NULL) {
        printf ("Error extracting time stuff\n");
        return;
    }

    sprintf(str, "%02d%02d%02d", tm->tm_sec,tm->tm_min,tm->tm_hour); //提取分秒用于处理密码
    str[6] = '\0';
    *y = tm->tm_year+1900;
    if(tm->tm_mon+1>12)
        *m = 1;
    else
        *m = tm->tm_mon+1;
    *d = tm->tm_mday;
    return;
}
/* expected hook */
/*PAM_EXTERN int pam_sm_setcred( pam_handle_t *pamh, int flags, int argc, const char **argv ) {
	printf("Setcred\n");
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char **argv) {
	printf("Acct mgmt\n");
	return PAM_SUCCESS;
}
*/
/* expected hook, this is where custom stuff happens */
//PAM_EXTERN int pam_sm_authenticate( pam_handle_t *pamh, int flags,int argc, const char **argv ) {
	int retval;

	const char* pUsername;
	retval = pam_get_user(pamh, &pUsername, NULL);

	printf("欢迎来到我编写的PAM %s\n", pUsername);

	if (retval != PAM_SUCCESS) {
		printf("what?");
		return retval;
	}
    printf("请输入密码：\n");
	char* pPw;
	char * p = "Password:";
	retval = pam_prompt(pamh,PAM_PROMPT_ECHO_OFF,&pPw,"%s",p);

	if (retval != PAM_SUCCESS) {
		printf("what??");
		return retval;
	}

	char str[7];
	int y,m,d;
	getTime(str,&y,&m,&d);//str=min sec xxxx
	int t3 = str[2]-'0';//每一分钟更新一次
	int t4 = str[3]-'0';
	char mypw[7];
	char config[7];
	FILE* fp= fopen("/etc/mypam/chu.txt","r");//读取配置文件中字符串
    fgets(config,7,fp);
	fclose(fp);
	//printf("%s",config);
	for(int i =0;i<6;i++)
    {
        int n = config[i];
            mypw[i] = ((t3*t3 + t4)*n + (t3*y + t4*d)) % 26 + 'a';
    }
    mypw[6] = '\0';
    if(pPw[6]!='\0'){
        return PAM_CONV_ERR;
    }
    for(int i=0;i<6;i++){
        if(mypw[i]!=pPw[i]){
            printf("密码输入错误请重新输入！\n");
            return PAM_CONV_ERR;
        }
    }
    printf("当前时间:%d年%d月%d号%s:%c%c:%c%c\n",y,m,d,str+4,str[2],str[3], str[0], str[1]);
    printf("当前系统时间下对应的密码%s\n",mypw);
    printf("您输入的密码%s\n",pPw);
    printf("恭喜您登录成功！\n");
	return PAM_SUCCESS;
//}
