int get_color(int *colors,int size,int rgb)
{
	int i;
	for(i=0;i<size;i++){
		if(colors[i]==rgb)
			return i;
		if(colors[i]==0xCCCCCCCC){
			colors[i]=rgb;
			break;
		}
	}
	return i;
}
int get_next_count(char *buf,int len,int *start_rgb)
{
	int i,start_color,max=16;
	if(len<16)
		max=len;
	for(i=0;i<max;i++){
		unsigned char r,g,b;
		int rgb;
		r=buf[i*3];
		g=buf[i*3+1];
		b=buf[i*3+2];
		rgb=(r<<16)|(g<<8)|b;
		if(i==0){
			start_color=rgb;
			if(start_rgb)
				start_rgb[0]=rgb;
		}
		else{
			if(start_color!=rgb)
				break;
		}
	}
	return --i;
}
int emit(FILE *fout,int letter)
{
	letter&=0xFF;
		fprintf(fout,"%c",letter);
		if(letter=='\\')
			fprintf(fout,"%c",letter);
}
int main()
{

	FILE *f,*fout;

	f=fopen("B:\\frame1.raw","rb");
	if(f){
		int i,len;
		int size=0x100000;
		int letter=0,index=0;
		int colors[20];
		char *buf=malloc(size);
		int q[2]={0};
		memset(colors,0xCC,sizeof(colors));
		fout=fopen("b:\\out.txt","wb");
		len=fread(buf,1,size,f);
		for(i=0;i<len;){
			int count,rgb;
			count=get_next_count(buf+i,len-i,&rgb);
			if(count>15)
				count=15;
			rgb=get_color(colors,sizeof(colors),rgb);
			if(rgb>3){
				rgb%=4;
			}
			printf("%i %i\n",count,rgb);
			letter=1;
			letter<<=6;
			letter|=(count<<2)|rgb;
			emit(fout,letter);
			index=0;
			i+=(count+1)*3;

		}
		fclose(fout);
		fclose(f);
	}
	getch();
}
