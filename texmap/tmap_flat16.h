/*
 THE COMPUTER CODE CONTAINED HEREIN IS THE SOLE PROPERTY OF OUTRAGE
 ENTERTAINMENT, INC. ("OUTRAGE").  OUTRAGE, IN DISTRIBUTING THE CODE TO
 END-USERS, AND SUBJECT TO ALL OF THE TERMS AND CONDITIONS HEREIN, GRANTS A
 ROYALTY-FREE, PERPETUAL LICENSE TO SUCH END-USERS FOR USE BY SUCH END-USERS
 IN USING, DISPLAYING,  AND CREATING DERIVATIVE WORKS THEREOF, SO LONG AS
 SUCH USE, DISPLAY OR CREATION IS FOR NON-COMMERCIAL, ROYALTY OR REVENUE
 FREE PURPOSES.  IN NO EVENT SHALL THE END-USER USE THE COMPUTER CODE
 CONTAINED HEREIN FOR REVENUE-BEARING PURPOSES.  THE END-USER UNDERSTANDS
 AND AGREES TO THE TERMS HEREIN AND ACCEPTS THE SAME BY USE OF THIS FILE.
 COPYRIGHT 1996-2000 OUTRAGE ENTERTAINMENT, INC.  ALL RIGHTS RESERVED.
 */
 #ifdef ACTIVATE_INNER_LOOP
		

			dest=(destptr+x1);

			int i;
                
			fix u  = LeftU;
            fix v  = LeftV;
			fix cu,cv;

			int srcptr;

			int tw=bm_w(Tex_bitmap,Current_mip);
			int th=bm_h(Tex_bitmap,Current_mip);

			TexSrc16=bm_data(Tex_bitmap,Current_mip);
						
			dest=(destptr+x1);
	
			for (i=0;i<width;i++)
			{
				cu=FixToInt(u);
				cv=FixToInt(v);
								
				cu&=(tw-1);
				cv&=(tw-1);
				srcptr=(cv*tw)+cu;

				if (TexSrc16[srcptr] & OPAQUE_FLAG)
					*dest = FLAT_SHADE_COLOR;
				
				dest++;
				
                u += dudx;
                v += dvdx;

			}
		
			
			
#endif