
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <math.h>

 extern int read_pgm_hdr(FILE *, int *, int *);
 extern void **matrix(int, int, int, int, int);
 extern void error(const char *);
 int skipNMS=0;
 float ** gauss_smooth;


 /*/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\//\*/

 /* READ PGM HEADER */

 /* This function reads the header of a PGM image. */
 /* The dimensions are returned as arguments. */
 /* This function ensures that there's no more than 8 bpp. */
 /* The return value is negative if there's an error. */

 int read_pgm_hdr(FILE *fp, int *nrows, int *ncols)
 {
   char filetype[3];
   int maxval;

   if(skipcomment(fp) == EOF
      || fscanf(fp, "%2s", filetype) != 1
      || strcmp(filetype, "P5")
      || skipcomment(fp) == EOF
      || fscanf(fp, "%d", ncols) != 1
      || skipcomment(fp) == EOF
      || fscanf(fp, "%d", nrows) != 1
      || skipcomment(fp) == EOF
      || fscanf(fp, "%d%*c", &maxval) != 1
      || maxval > 255)
     return(-1);
   else return(0);
 }

 /*------------------------------------------------------------------------------*/
 /* ERROR HANDLER */

 void error(const char *msg)
 {
   fprintf(stderr, "%s\n", msg);
   exit(1);
 }

 /*-------------------------------------------------------------------------------*/

 /* DYNAMICALLY ALLOCATE A PSEUDO 2-D ARRAY */

 /* This function allocates a pseudo 2-D array of size nrows x ncols. */
 /* The coordinates of the first pixel will be first_row_coord and */
 /* first_col_coord. The data structure consists of one contiguous */
 /* chunk of memory, consisting of a list of row pointers, followed */
 /* by the array element values. */
 /* Assumption:  nrows*ncols*element_size, rounded up to a multiple  */
 /* of sizeof(long double), must fit in a long type.  If not, then */
 /* the "i += ..." step could overflow.                              */

 void **matrix(int nrows, int ncols, int first_row_coord,
               int first_col_coord, int element_size)
 {
   void **p;
   int alignment;
   long i;

   if(nrows < 1 || ncols < 1) return(NULL);
   i = nrows*sizeof(void *);
   /* align the addr of the data to be a multiple of sizeof(long double)
   alignment = i % sizeof(long double);
   if(alignment != 0) alignment = sizeof(long double) - alignment;
   i += nrows*ncols*element_size+alignment;
   if((p = (void **)malloc((size_t)i)) != NULL)
   {
     /* compute the address of matrix[first_row_coord][0] */
     p[0] = (char *)(p+nrows)+alignment-first_col_coord*element_size;
     for(i = 1; i < nrows; i++)
       /* compute the address of matrix[first_row_coord+i][0] */
       p[i] = (char *)(p[i-1])+ncols*element_size;
     /* compute the address of matrix[0][0] */
     p -= first_row_coord;
   }
   return(p);
 }

 /*-----------------------------------------------------------------------------------------------------------------*/
 /* SKIP COMMENT */

 /* This function skips past a comment in a file. The comment */
 /* begins with a '#' character and ends with a newline character. */
 /* The function returns EOF if there's an error. */

 int skipcomment(FILE *fp)
 {
   int i;

   if((i = getc(fp)) == '#')
     while((i = getc(fp)) != '\n' && i != EOF);
   return(ungetc(i, fp));
 }


 /*----------------------------------------------------------------------------------------------------------------*/

 /* REFLECT AN IMAGE ACROSS ITS BORDERS */

 /* The parameter "amount" tells the number of rows or columns to be */
 /* reflected across each of the borders. */
 /* It is assumed that the data type is unsigned char. */
 /* It is assumed that the array was allocated to be of size at least */
 /* (nrows+2*amount) by (ncols+2*amount), and that the image was loaded */
 /* into the middle portion of the array, with coordinates, */
 /*      0 <= row < nrows, 0 <= col < ncols */
 /* thereby leaving empty elements along the borders outside the image */
 /* The "reflect" function will then fill in those empty */
 /* elements along the borders with the reflected image pixel values. */
 /* For example, x[0][-1] will be assigned the value of x[0][0], */
 /* and x[0][-2] will be assigned the value of x[0][1], if amount=2. */

 void reflect(unsigned char **xc, int nrows, int ncols, int amount)
 {
   int i, j;

   if(matrix == NULL || nrows < 1 || ncols < 1 || amount < 1
     || amount > (nrows+1)/2 || amount > (ncols+1)/2)
     error("reflect: bad args");

   for(i = -amount; i < 0; i++)
   {
     for(j = -amount; j < 0; j++)
       xc[i][j] = xc[-i-1][-j-1];
     for(j = 0; j < ncols; j++)
       xc[i][j] = xc[-i-1][j];
     for(j = ncols; j < ncols+amount; j++)
       xc[i][j] = xc[-i-1][ncols+ncols-j-1];
   }
   for(i = 0; i < nrows; i++)
   {
     for(j = -amount; j < 0; j++)
       xc[i][j] = xc[i][-j-1];
     for(j = ncols; j < ncols+amount; j++)
       xc[i][j] = xc[i][ncols+ncols-j-1];
   }
   for(i = nrows; i < nrows+amount; i++)
   {
     for(j = -amount; j < 0; j++)
       xc[i][j] = xc[nrows+nrows-i-1][-j-1];
     for(j = 0; j < ncols; j++)
       xc[i][j] = xc[nrows+nrows-i-1][j];
     for(j = ncols; j < ncols+amount; j++)
       xc[i][j] = xc[nrows+nrows-i-1][ncols+ncols-j-1];
   }
 }
 /*----------------------------------------------------------------------------------------*/

 /* REFLECTING FLOAT */

 void reflectf(float **xc, int nrows, int ncols, int amount)
 {
  int i, j;

   if(matrix == NULL || nrows < 1 || ncols < 1 || amount < 1
     || amount > (nrows+1)/2 || amount > (ncols+1)/2)
     error("reflect: bad args");


   for(i = -amount; i < 0; i++)
   {
     for(j = -amount; j < 0; j++)
       xc[i][j] = xc[-i-1][-j-1];
     for(j = 0; j < ncols; j++)
       xc[i][j] = xc[-i-1][j];
     for(j = ncols; j < ncols+amount; j++)
       xc[i][j] = xc[-i-1][ncols+ncols-j-1];
   }

   for(i = 0; i < nrows; i++)
   {
     for(j = -amount; j < 0; j++)
       xc[i][j] = xc[i][-j-1];
     for(j = ncols; j < ncols+amount; j++)
       xc[i][j] = xc[i][ncols+ncols-j-1];
   }

   for(i = nrows; i < nrows+amount; i++)
   {
     for(j = -amount; j < 0; j++)
       xc[i][j] = xc[nrows+nrows-i-1][-j-1];
     for(j = 0; j < ncols; j++)
       xc[i][j] = xc[nrows+nrows-i-1][j];
     for(j = ncols; j < ncols+amount; j++)
       xc[i][j] = xc[nrows+nrows-i-1][ncols+ncols-j-1];
   }
 }

 /*--------------------------------------------------------------------------------------------------------------*/

 /* A method for Linear Interpolation */

 float LinearInterpolate(
    float y1,float y2,
    float mu)
 {
    return(y1*(1-mu)+y2*mu);
 }

 /*--------------------------------------------------------------------------------------*/
 void nms(float **anglec,float **magc,float **dd_image,int nrows ,int ,int thresh,int thresh2)
 {


   /* PERFORMS NON-MAXIMUM SUPRESSION TO DETECT EDGES */

   int ,c,edgepoints1=0,edgepoints2=0,last=0,nochange,row,col,count,run;
   FILE *fpy;
   float angle,maxm;
   int ax_pos,ay_pos,r1,c1,r2,c2,edgecount,r3,c3,r4,c4,count_points=0;
   unsigned char** edgemap;
   float mu =0.5 , anglecal, ivalue1,ivalue2;
   f("============================================================\n");
   printf(" Performing Non_maximum-Supression\n");


   edgemap = (unsigned char **) matrix(nrows+2,ncols+2,-1,-1,sizeof();
   /* SO FIRST REFLECT THE MAGC BY 1 PIXEL */
   printf("-------------------------------------------------\n");
   printf("Finished processing: Reflected magnitude image by 1 pixel\n");
   printf("Finished processing: Reflected angle image by 1 pixel\n");

  /* HYSTERESIS THRESHODING */

       for(i=0;i<nrows;i++)
         {
           for(j=0;j<ncols;j++)
             {
               edgemap[i][j] =0;
             }
         }

       /* ACTUAL PROCESSING */
       if(thresh != thresh2)
         {
           while( nochange)
             {
                printf( "Run %d \n",run);
               count_points =0;
               edgepoints2 =0;

               for(i=0;i<ncols;i++)
                 {
                   for(j=0;j<nrows;j++)
                     {

                       count =0;

                       for(row = i-1; row<= i+1; row++)
                         {
                           for(col = j-1; col <= j+1; col++)
                             {

                               if((dd_image[i][j] * dd_image[row][col] <  (abs(dd_image[i][j] <= abs(dd_image[row][col])))))
                                 count ++;
                             }
                         }
                       if(count >= 1)
                         {

                           if(magc[i][j] >= thresh2 *thresh2)
                             {
                               edgemap[i][j] = 255;
                               edgepoints2++;
                             }
                           else if(magc[i][j] <= thresh*thresh)
                             {
                               edgemap[i][j] = 0;
                             }
                           else
                             {
                               edgecount =0;
                               for(r=i-1;r<= i+1; r++)
                                 {
                                   for(c=j-1;c<=j+1;c++)
                                     {
                                       if(edgemap[r][c] == 255 )
                                         edgecount++;
                                     }
                                 }
                               if(edgecount >= 1)
                                 {
                                   edgemap[i][j] = 255;
                                   edgepoints2++;
                                 }
                               else
                                 edgemap[i][j] =0;
                             }
                         }
                     }
                 }

               /* CONDITION FOR CONVERGENCE */
               if(abs(last-edgepoints2) <= 100)
                 nochange=0; /* means there is nochange stop */
               else
                 nochange =1; /* means there is change -> iterate */
               last = edgepoints2;
               count_points = count_points + edgepoints2;
               edgepoints2 =0;
               run +=1;

             }
           run +=1;
         }



 printf(" Number of Edgepoints after hysterisis thresholding is %d ast );
   if(last < 13000)
     {
       f("===========================================================\n");
       printf(" Reduce lower threshold or Upper threshold and try again
       exit(0);
     }
   printf(" Finsihed calculating the edges using thresholding and NMS\n");

  /* WRITE THE IMAGE AFTER HYSTERISIS THRESHOLDING*/

   if((fpy =fopen("ecanny_map.pgm","w"))== 0)
          error(" Error writing file\n");
   fprintf(fpy, "P5\n%d %d\n255\n", ncols, nrows);
   for(i = 0; i < nrows; i++)
     if(fwrite(&edgemap[i][0], sizeof(char), ncols, fpy) != ncols)
       error("can't write the image");



   if((last > 12000 && last < 15000))
     {
       printf(" # Edgepoints within the range specified\n");
     }
   else if(last < 12000 )
     {
       thresh = thresh -5;
       thresh2 = thresh2 -5;
       nms(anglec,magc,dd_image,nrows,ncols,thresh,thresh2);
     }
   else
     {
       thresh = thresh + 5;
       thresh2 = thresh2 +5;
       nms(anglec,magc,dd_image,nrows,ncols,thresh,thresh2);
     }

   printf("The lower threshold used = %d \n",thresh);
    printf("The Upper threshold used = %d \n",thresh2);
    printf("==========================================\n");
    count_points =0;
 }




 /*------------------------------------------------------------------------------------------/
 /* GAUSSIAN SMOOTHING */

 /* Smooth thr image by a gaussian kernel which depends on the sigma */
 /* User driven sigma */
 /* store in a global array gauss_smooth_x */

 int gauss( unsigned char **xc, int nrows, int ncols, float sigma,int )
 {
   FILE *fpy;
   float  **gauss_filter,sum;
   unsigned char ** write_smooth_image;
   int grows,gcols,set,i,j,r,c;
   f("----------------------------------------------------------------\n");
   printf(" Performing Gaussian Smoothing using sigma of %f \n", sigma);


   /* OPEN FILE FOR IMAGE WRITE */
   if((fpy =fopen("smoothed_image.pgm","w"))== 0)
     error(" Error writing file\n");
   printf("---------------------------------------------------------------n");


   /* DEFINE A GAUSSIAN FILTER KERNEL */

   printf(" DEFINE A GAUSSIAN KERNEL\n");

   /* ALLOCATE MEMORY */

   grows =4*scale +1;
   gcols =4*scale +1;
   printf(" ALLOCATE MEMORY\n");
   gauss_filter = (float **)matrix(grows, gcols, -2*scale, -2*scale, f(float));
   printf("ALLOCATED MEMORY\n");

   set = -scale;
   /*  ALLOCATE MEMORY FOR GAUSS SMOOTH */
   printf(" ALLOCATE MEMORY FOR GAUSS SMOOTH\n");
   gauss_smooth = (float **)matrix(nrows-4*set, ncols-4*set, 2*set, , sizeof(float));
   write_smooth_image = (unsigned char **)matrix(nrows, ncols, 0, 0, f(char));
   printf("ALLOCATED MEMORY FOR GAUSS SMOOTH\n");

   sum =0;

   reflectf(gauss_smooth,nrows,ncols,2*scale);

   /* FILL IN THE VALUES FOR THE GAUSSIAN FILTER */
   printf("------------------------------------------------------\n");
   printf(" FILLING UP GAUSS FILTER VALUES\n");
   for( i=-(gcols/2); i<= gcols/2; i++)
     {
       for(j=-(grows/2);j<= gcols/2; j++)
         {
           gauss_filter[i][j] = (1/(2*M_PI*sigma*sigma))*exp(-((i*i)+ (/(2*sigma*sigma));
           sum = sum + gauss_filter[i][j];
         }
     }
   /* MAKING DC-BIAS ZERO */
    for( i=-(gcols/2); i<= gcols/2; i++)
     {
       for(j=-(grows/2);j<= gcols/2; j++)
         {
           gauss_filter[i][j] = gauss_filter[i][j];
         }
     }
   printf(" FILLED UP GAUSS FILTER VALUES\n");

  /* INITIALIZE GAUSS SMOOTH*/
   printf("---------------------------------------------------------\n");
   printf("Initializing Gauss smooth matrix\n");
   for(i=set;i<nrows; i++)
     {
       for(j=set;j< ncols; j++)
         {
           gauss_smooth[i][j] =0;

         }
     }

   /* CONVOLVE WITH GAUSSIAN FILTER AND STORE IN GAUSS_SMOOTH */
   printf(" -------------------------------------------------------\n");
   printf(" CONVOLUTION WITH GAUSS FILTER\n");
   for(i=0;i<nrows; i++)
     {
       for(j=0;j< ncols; j++)
         {
           /* CONVOLUTION*/

           for( c=set; c<= -set; c++)
             {
               for(r=set;r<= -set; r++)
                 {
                   gauss_smooth[i][j] = gauss_smooth[i][j] + (float) xc[j+r] * gauss_filter[c][r];

                 }
             }
         }
     }

   /* CHAR THE IMAGE FOR WRITING */
    printf(" CONVOLUTION WITH GAUSS FILTER\n");
   for(i=0;i<nrows; i++)
     {
       for(j=0;j< ncols; j++)
         {

           write_smooth_image[i][j] = (char) (gauss_smooth[i][j]);
          /*  gauss_smooth[i][j] =  gauss_smooth[i][j] * dc_gain[i][j]; */
         }
     }

   /* WRITE THE IMAGE */
   fprintf(fpy, "P5\n%d %d\n255\n", ncols, nrows);
   for(i = 0; i < nrows; i++)
     if(fwrite(&write_smooth_image[i][0], sizeof(char), ncols, fpy) != )
       error("can't write the image");

   /* CLOSE FILE & QUIT */
   fclose(fpy);
   return 1;


 }





 ------------------------------------------------------------------------*/

 /* EDGE DETECTION BY CANNY OPERATOR */

 /* The yc array is where the magnitude of the resultant correlation is d*/
 /* The zc array is where the gradient of the resultant correlation is d*/
 /* The edgec array is where edge/ not an edge info is stored in 1's or /

 void canny(unsigned char **xc, int nrows, int ncols, int thresh,int h2,float sigma,int scale)
 {

   int r,c,i,j,set,edgepoints2,last,nochange=1,edgecount,check,count,row,cn,t1,t2;
   int count_points =0;
   float **fx,**fy,**fxy,**fxx,**fyy,**gradient, **dd_image,**anglec;
   unsigned char **edgemap;
   FILE *fpy;
   printf(" Edge detection will be performed by Canny method\n");
   t1 = thresh;
   t2 = thresh2;

   printf("---------------------------------------------------------------n");


   /* STEP: 1 --- GAUSSIAN SMOOTHING */

   if(check = gauss( xc, nrows,ncols, sigma,scale) == 0)
     error(" Error in gauss smoothing function\n");

   /* ALLOCATING MEM */

   set = scale;
   fx = (float **)matrix(nrows+4*scale, ncols+4*scale, -2*scale,-2*scale, f(float));
   fy = (float **)matrix(nrows+4*scale, ncols+4*scale, -2*scale, ale, sizeof(float));
   fxy = (float **)matrix(nrows+4*set, ncols+4*set, -2*set, -2*set, sizeoft));
   fxx = (float **)matrix(nrows+4*set, ncols+4*set, -2*set, -2*set, sizeoft));
   fyy = (float **)matrix(nrows+4*set, ncols+4*set, -2*set, -2*set, sizeoft));
   gradient =(float **)matrix(nrows+2*set, ncols+2*set, -2*set, -2*set, f(float));
   dd_image=(float **)matrix(nrows+4*set, ncols+4*set, -2*set, -2*set, f(float));
   anglec = (float **)matrix(nrows+4*set, ncols+4*set, -2*set, -2*set, f(float));

   reflectf(fx,nrows,ncols,scale);
   reflectf(fy,nrows,ncols,scale);
   reflectf(fxy,nrows,ncols,scale);
   reflectf(dd_image,nrows,ncols,scale);

   edgemap = (unsigned char **)matrix(nrows, ncols, 0, 0, sizeof(char));
   printf("-------------------------------------------\n");
   printf(" Allocated memory for edgemap\n");

   /* FIND THE LoG OF THE SMOOTHED IMAGE */

   for(i=0;i<ncols;i++)
     {
       for(j=0;j<nrows;j++)
         {

           fx[i][j] = gauss_smooth[i][j+1] - gauss_smooth[i][j-1];
           fy[i][j] = gauss_smooth[i+1][j] - gauss_smooth[i-1][j];
           fxy[i][j] = gauss_smooth[i-1][j-1]+gauss_smooth[i+1][j-1] - _smooth[i+1][j+1] - gauss_smooth[i-1][j-1];
           fxx[i][j] = gauss_smooth[i][j+2]- 2*gauss_smooth[i][j] + _smooth[i][j-2];
           fyy[i][j] = gauss_smooth[i+2][j] - 2*gauss_smooth[i][j] + _smooth[i-2][j];
           gradient[i][j] = fx[i][j] * fx[i][j] + fy[i][j]*fy[i][j];
           dd_image[i][j] =(fx[i][j] * fx[i][j] * fxx[i][j] + 2*fx[i][[i][j]*fxy[i][j] + fy[i][j]*fy[i][j]* fyy[i][j])/gradient[i][j];


         }
     }

   /* Non-Maximum Supression */
   nms(anglec,gradient,dd_image,nrows,ncols,thresh,thresh2);


 }




 /* ===============================================================================*/

 int main(int argc, char **argv)
 {
   FILE *fpx, *fpy;
   int set,nrows, ncols, i, j,thresh,thresh2,scale,c=0;
   float sigma;
   unsigned char **x;
   char *str;
   int r=0,p=0,s=0,f=0,histh=0;


   /* PARSE THE COMMAND LINE */
   while(--argc > 0 && **(argv+1) == '-')
     switch((*++argv)[1])
     {

     case 's':

           skipNMS =1;
           printf("skipNMS\n");


     case 't':
       printf("Setting threshold \n");
       printf(" %s is the threshold\n", *++argv);
       str = *(argv);
       thresh = atoi(str);
       str = *(++argv);
       printf(" The str is %s\n",str);

       if(str[0] <= '9' && str[0] >= '0')
         {
            histh =1;
            printf("Performing Hysteresis Thresholding\n");
            thresh2 = atoi(str);
         }
         else
         {
            histh =0;
            printf("Using only one global threshold\n");
            thresh2 = thresh;
            printf(" Mention the upper threshold also\n");
            error("Usage:edge {-roberts,-prewitt,-sobel,-frei} [-skipNMS] hresh_low thresh_high] img ");
            exit(0);
         }

       break;

     case 'c':
       printf(" Proceeding with Canny Edge-detection algorithm\n");
       c =1;
       break;

     default:
       error("Usage:edge {-roberts,-prewitt,-sobel,-frei} [-skipNMS] [-t h_low thresh_high] img ");
       break;


     }   /* end of case statement */


   /* IF CANNY ASK USER FOR SIGMA VALUE */
   if(c)
     {
       printf(" Choose a sigma value ( Caorseness parameter) :");
       scanf("%f",&sigma);
       printf(" Choose a scale for the sigma: ");
       scanf("%d",&scale);
     }

   /* OPEN FILES */
   printf("------------------------------------------------\n");
   printf("Opening image file\n");
   if(argc == 0) fpx = stdin;
   else
    {
      printf(" Making decesion\n");
      if(histh)
         {
           if((fpx = fopen(*(++argv), "r")) == NULL)
           {
                   printf("%s1\n",(*argv));
                   error("can't open file");
           }
         }
         else
         {
           if((fpx = fopen((str), "r")) == NULL)
           {
              printf("%s2\n",(*argv));
              error("can't open file");
           }
         }
    }
   printf("-----------------------------------------\n");
   printf(" Opened file --image file %s \n", *argv);
   fpy = stdout;

   /* READ HEADER */
   if(read_pgm_hdr(fpx, &nrows, &ncols) < 0)
     error("not a PGM image or bpp > 8");
   printf("------------------------------------------\n");
   printf(" Read Header \n");


   /* ALLOCATE ARRAYS */
   /* The x array is extended by one pixel on all four sides. */
   /* The first element of the array is x[-1][-1]. */
   /* The first actual pixel of the image will be stored at x[0][0]. */
   /* y is the output image -- magnitude */
   /* z is the output image -- gradient  */
   /* edge_map is the matrix containbing whether a given pixel is an edge t */

   x = (unsigned char **)matrix(nrows+2*scale, ncols+2*scale, -scale, e, sizeof(char));

   printf("--------------------------------------------------------\n");
   printf("Allocated arrays for output image:gradient and magnitude and map\n");
   if(x == NULL) error("can't allocate memory");


   /* READ THE IMAGE */
   for(i = 0; i < nrows; i++)
     if(fread(&x[i][0], sizeof(char), ncols, fpx) != ncols)
       error("can't read the image");
   printf("----------------------------------------------\n");
   printf(" Read image: %s\n",*argv);

   /* REFLECT THE IMAGE ACROSS ITS BORDERS BY ONE PIXEL */
   reflect(x, nrows, ncols, scale);
   printf("-------------------------------------------------\n");
   printf("Finished processing: Reflected image by 1 pixel\n");
   printf("TEST %d \n",x[-1][-1]);

   /* DECIDE ON NON-MAXIMUM SUPRESSION */

   if(skipNMS)
     {
       printf("----------------------------------------------\n");
       printf(" Skip Non-Maximum-Supression\n");
     }
   else
     {
       printf("-----------------------------------------------\n");
       printf(" Performing NMS \n");
     }
   /* DECIDE ON EDGE DETECTION METHOD */

   if(c ==1)
     {
       printf("-----------------------------\n");
       printf(" Apply Canny Edge Detector\n");
       canny(x,nrows,ncols,thresh,thresh2,sigma,scale);
     }

   fclose(fpx);
   return 1;
 }
