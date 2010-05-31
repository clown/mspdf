#include <cstdio>
#include <cstdlib>
#include <cstring>

/* This program takes a ttc file and turns it into its component ttf files */
/* The makes two changes to the data:									   */
/*		* The tables are placed a different offsets 					   */
/*		  (so the offset fields in the table header are different)		   */
/*		* the checksumAdjustment field of the 'head' table is set correctly*/

#define CHR(ch1,ch2,ch3,ch4) (((ch1)<<24)|((ch2)<<16)|((ch3)<<8)|(ch4))

inline void putshort(FILE *file,int sval) {
	putc((sval>>8)&0xff,file);
	putc(sval&0xff,file);
}

inline void putlong(FILE *file,int val) {
	putc((val>>24)&0xff,file);
	putc((val>>16)&0xff,file);
	putc((val>>8)&0xff,file);
	putc(val&0xff,file);
}

inline int getushort(FILE *ttf) {
	int ch1 = getc(ttf);
	int ch2 = getc(ttf);
	if ( ch2==EOF )
		return( EOF );
		return( (ch1<<8)|ch2 );
}

inline int getlong(FILE *ttf) {
	int ch1 = getc(ttf);
	int ch2 = getc(ttf);
	int ch3 = getc(ttf);
	int ch4 = getc(ttf);
	if ( ch4==EOF )
		return( EOF );
		return( (ch1<<24)|(ch2<<16)|(ch3<<8)|ch4 );
}

inline unsigned int filecheck(FILE *file) {
	unsigned int sum = 0, chunk;

	rewind(file);
	while ( 1 ) {
				chunk = getlong(file);
				if ( feof(file) || ferror(file))
		break;
				sum += chunk;
	}
		return( sum );
}

inline void copytable(FILE *ttf,FILE *ttc,int offset,int length) {
	int i, ch;

	fseek(ttc,offset,SEEK_SET);
	for ( i=0; i<length && (ch=getc(ttc))!=EOF ; ++i ) putc(ch,ttf);
	if ( ch==EOF ) fprintf( stderr, "File ended before table\n" );
	if ( length&1 ) {
				putc('\0',ttf);
				++length;
	}
	if ( length & 2 ) putshort(ttf,0);
}

inline void handlefont(const char *filename,int which,FILE *ttc,int offset) {
	char outfile[2000];
	FILE *ttf;
	int i, cnt, *offsets, *lengths, head, tag, pos, headpos;

	strcpy(outfile,filename);
#if 0
	char* pt = strrchr(outfile,'.');
	if ( pt==NULL )
		pt = outfile + strlen(outfile);
	sprintf( pt, "_%02d.ttf", which );
#endif
	ttf = fopen( outfile,"wb");
	if ( ttf==NULL ) {
		fprintf( stderr, "Failed to open %s for output.\n", outfile );
return;
	}

	fseek(ttc,offset,SEEK_SET);
	putlong(ttf,getlong(ttc));			/* sfnt version */
	putshort(ttf,cnt = getushort(ttc)); /* table cnt */
	putshort(ttf,getushort(ttc));		/* binary search header */
	putshort(ttf,getushort(ttc));
	putshort(ttf,getushort(ttc));

	offsets = (int*)malloc(cnt*sizeof(int));
	lengths = (int*)malloc(cnt*sizeof(int));
	head = -1;
	for ( i=0; i<cnt; ++i ) {
		tag = getlong(ttc);
		if ( tag==CHR('h','e','a','d'))
			head = i;
		putlong(ttf,tag);
		putlong(ttf,getlong(ttc));		/* checksum */
		putlong(ttf,offsets[i] = getlong(ttc)); 		/* Reserve space for offset, will fix later */
		putlong(ttf,lengths[i] = getlong(ttc));
	}
	headpos = -1;
	for ( i=0; i<cnt; ++i ) {
		pos = ftell(ttf);
		copytable(ttf,ttc,offsets[i],lengths[i]);
		if ( i==head ) {
			fseek(ttf,pos+8,SEEK_SET);
			putlong(ttf,0);
			headpos = pos;
		}
		fseek(ttf,12+i*16+8,SEEK_SET);
		putlong(ttf,pos);								/* Fix offset here */
		fseek(ttf,0,SEEK_END);
	}
	if ( headpos!=-1 ) {
		unsigned int checksum;
		checksum = filecheck(ttf);
		checksum = 0xb1b0afba-checksum;
		fseek(ttf,headpos+2*sizeof(int),SEEK_SET);
		putlong(ttf,checksum);
	}
	fclose(ttf);
	free(offsets); free(lengths);
}
