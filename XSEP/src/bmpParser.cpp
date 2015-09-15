#include "bmpParser.h"

bmpParser::bmpParser()
{
	this->header = new bmpFileHeader;
	this->info = new bmpInfoHeader;
	this->the_image=NULL;
	this->the_pallette=NULL;
	this->byte_buffer=NULL;
}
 
bmpParser::~bmpParser()
{
	delete[] this->byte_buffer;
	delete[] this->the_pallette;
	for(int i=0;i<(int) this->info->biHeight;i++) delete[] the_image[i];
	delete[] the_image;
	delete this->header;
	delete this->info;	
}

void bmpParser::analyzeHeader(char *buffer)
{
	unsigned char temp[40];

	memcpy(temp,buffer,14);
	this->readHeader(temp);
	memcpy(temp,&buffer[14],40);
	this->readInfo(temp);

	if(this->info->biBitCount != 24)
	{
		printf("Using palette\n");		
		this->pallette_length=(1 << info->biBitCount)<< 2;
		if(this->the_pallette == NULL)this->the_pallette = new unsigned char [this->pallette_length];
		memcpy(this->the_pallette,&buffer[54],this->pallette_length);
		printf("Palette read with %d entries\n",this->pallette_length);
	}
	else this->pallette_length=0;

}

void bmpParser::analyzeBuffer(char *buffer)
{		
	this->byte_buffer_length = (this->info->biWidth * this->info->biHeight * this->info->biBitCount) / 8;
	int line_size = this->info->biWidth * this->info->biBitCount / 8;	
	int step = line_size % 4;
	int indy1,indy2,i,j;

	if(step != 0 ) printf("Padding done \n");
	else printf("Padding not done \n");
	line_size -=step;

	printf("We should read %d bytes \n",this->byte_buffer_length);
	
	if(this->byte_buffer == NULL)
	{
		this->byte_buffer = new unsigned char[this->byte_buffer_length];	
	}
	
	indy1=54+this->pallette_length;
	indy2=0;

	for(i=0;i<(int) info->biHeight;i++)
	{
		for(j=0;j<line_size;j++,indy1++,indy2++)
		{
			byte_buffer[indy2] = buffer[indy1];
		}
		indy1+=step;
	}

	if(this->the_image == NULL)
	{
		this->the_image = new unsigned char * [info->biHeight];	
	
		line_size = 3*info->biWidth;
		for(i=0;i< (int) this->info->biHeight;i++) the_image[i]=new unsigned char [line_size];
	}
	
	if(drawIt() == -1)
	{
			printf("Unsupported bitBount. Aborting ... \n");
			exit(1);	
	}	
}

void bmpParser::readHeader(unsigned char *buff)
{
	memcpy(this->header->name,buff,2);
	header->name[2]='\0';
	memcpy((unsigned char *) &this->header->bfSize,&buff[2],4);
	memcpy((unsigned char *) &this->header->bfReserved1,&buff[6],2);
	memcpy((unsigned char *) &this->header->bfReserved2,&buff[8],2);
	memcpy((unsigned char *) &this->header->bfOffBits,&buff[10],4);	
}

void bmpParser::readInfo(unsigned char *buff)
{
	memcpy((char *) &this->info->biSize,buff,4);
	memcpy((char *) &this->info->biWidth,&buff[4],4);
	memcpy((char *) &this->info->biHeight,&buff[8],4);
	memcpy((char *) &this->info->biPlanes,&buff[12],2);
	memcpy((char *) &this->info->biBitCount,&buff[14],2);
	memcpy((char *) &this->info->biCompression,&buff[16],4);
	memcpy((char *) &this->info->biSizeImage,&buff[20],4);
	memcpy((char *) &this->info->biXPelsPerMeter,&buff[24],4);
	memcpy((char *) &this->info->biYPelsPerMeter,&buff[28],4);
	memcpy((char *) &this->info->biClrUsed,&buff[32],4);
	memcpy((char *) &this->info->biClrImportant,&buff[36],4);
}

int bmpParser::drawIt()
{
	int buff_size;
	unsigned char *pixel,*runner;
	int i,j,k,mask,col_index;
	int row_size;
	
	row_size = 3 * this->info->biWidth;	
	for(mask=0,i=0;i<this->info->biBitCount;i++)	mask = (mask << 1) | 1;
	
	runner = this->byte_buffer;
	buff_size = this->byte_buffer_length;
	
	if(this->info->biBitCount==24)
	{
		for(i=0;i<(int) this->info->biHeight;i++)
		{		
			pixel=this->the_image[i];
			for(j=0;j<row_size;j+=3,pixel+=3,runner+=3)
			{
				*pixel = *runner;
				*(pixel+1) = *(runner+1);
				*(pixel+2) = *(runner+2);				
			}
		}
	}
	else
	{
		if((this->info->biBitCount==8) || (this->info->biBitCount==4) || (this->info->biBitCount==1))
		{	
			for(i=0,j=0;j<buff_size;j++,runner++)
			{
				for(k=0;k<8;k+=this->info->biBitCount,i+=3)
				{
					pixel = &this->the_image[i / row_size][i % row_size];
					col_index =	(*runner & mask) << 2;
					*(pixel+1) = this->the_pallette[col_index];
					*pixel = this->the_pallette[col_index+1];
					*(pixel+2) = this->the_pallette[col_index+2];					
					*runner = (unsigned char) (*runner >> this->info->biBitCount);
				}
			}
		}
		else return -1;
	}				
	return 0;
}
