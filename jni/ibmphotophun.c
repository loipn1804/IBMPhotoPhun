/*
 * IBMPhotoPhun.c
 * 
 * Author: Frank Ableson
 * Contact Info: fableson@navitend.com
 */
// plus
#include <string.h>
#include <stdio.h>
// plus
#include <jni.h>
#include <android/log.h>
#include <android/bitmap.h>

#define  LOG_TAG    "libibmphotophun"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

typedef struct {
	uint8_t alpha;
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} argb;

/*
 convertToGray
 Pixel operation
 */
JNIEXPORT void JNICALL Java_com_msi_ibm_ndk_CameraSurfaceView_convertToGray(JNIEnv * env, jobject  obj, jobject bitmapcolor,jobject bitmapgray)
{
    AndroidBitmapInfo  infocolor;
    void*              pixelscolor; 
    AndroidBitmapInfo  infogray;
    void*              pixelsgray;
    int                ret;
    int 			y;
    int             x;

     
    LOGI("convertToGray");
    if ((ret = AndroidBitmap_getInfo(env, bitmapcolor, &infocolor)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return;
    }

    
    if ((ret = AndroidBitmap_getInfo(env, bitmapgray, &infogray)) < 0) {
        LOGE(
		"AndroidBitmap_getInfo() failed ! error=%d", ret);
        return;
    }

    
    LOGI(
		"color image :: width is %d; height is %d; stride is %d; format is %d;flags is %d",
		infocolor.width, infocolor.height, infocolor.stride, infocolor.format,
		infocolor.flags);
    if (infocolor.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        LOGE("Bitmap format is not RGBA_8888 !");
        return;
    }

    
    LOGI(
		"gray image :: width is %d; height is %d; stride is %d; format is %d;flags is %d",
		infogray.width, infogray.height, infogray.stride, infogray.format,
		infogray.flags);
    if (infogray.format != ANDROID_BITMAP_FORMAT_A_8) {
        LOGE("Bitmap format is not A_8 !");
        return;
    }
  
    
    if ((ret = AndroidBitmap_lockPixels(env, bitmapcolor, &pixelscolor)) < 0) {
        LOGE(
		"AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }

    if ((ret = AndroidBitmap_lockPixels(env, bitmapgray, &pixelsgray)) < 0) {
        LOGE(
		"AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }

    // modify pixels with image processing algorithm
    
    for (y=0;y<infocolor.height;y++) {
    	argb * line = (argb *) pixelscolor;
    	uint8_t * grayline = (uint8_t *) pixelsgray;
    	for (x=0;x<infocolor.width;x++) {
    		grayline[x] = 0.3 * line[x].red + 0.59 * line[x].green + 0.11*line[x].blue;
    	}
    	
    	pixelscolor = (char *)pixelscolor + infocolor.stride;
    	pixelsgray = (char *) pixelsgray + infogray.stride;
    }
    
    LOGI(
		"unlocking pixels")
;
    AndroidBitmap_unlockPixels(env, bitmapcolor);
    AndroidBitmap_unlockPixels(env, bitmapgray);

    
}



/*
findEdges
Matrix operation
*/
JNIEXPORT void JNICALL Java_com_msi_ibm_ndk_CameraSurfaceView_findEdges(JNIEnv * env, jobject  obj, jobject bitmapgray,jobject bitmapedges)
{
    AndroidBitmapInfo  infogray;
    void*              pixelsgray;
    AndroidBitmapInfo  infoedges;
    void*              pixelsedge;
    int                ret;
    int 			y;
    int             x;
    int 			sumX,sumY,sum;
    int 			i,j;
    int				Gx[3][3];
    int				Gy[3][3];
    uint8_t			*graydata;
    uint8_t			*edgedata;
    

	LOGI("findEdges running");    
    
    Gx[0][0] = -1;Gx[0][1] = 0;Gx[0][2] = 1;
    Gx[1][0] = -2;Gx[1][1] = 0;Gx[1][2] = 2;
    Gx[2][0] = -1;Gx[2][1] = 0;Gx[2][2] = 1;
    
    
    
    Gy[0][0] = 1;Gy[0][1] = 2;Gy[0][2] = 1;
    Gy[1][0] = 0;Gy[1][1] = 0;Gy[1][2] = 0;
    Gy[2][0] = -1;Gy[2][1] = -2;Gy[2][2] = -1;


    if ((ret = AndroidBitmap_getInfo(env, bitmapgray, &infogray)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return;
    }

    
    if ((ret = AndroidBitmap_getInfo(env, bitmapedges, &infoedges)) < 0) {
        LOGE(
		"AndroidBitmap_getInfo() failed ! error=%d", ret);
        return;
    }

    
    
    LOGI(
		"gray image :: width is %d; height is %d; stride is %d; format is %d;flags is %d",
		infogray.width, infogray.height, infogray.stride, infogray.format,
		infogray.flags);
    if (infogray.format != ANDROID_BITMAP_FORMAT_A_8) {
        LOGE("Bitmap format is not A_8 !");
        return;
    }
  
    LOGI(
		"color image :: width is %d; height is %d; stride is %d; format is %d;flags is %d",
		infoedges.width, infoedges.height, infoedges.stride, infoedges.format,
		infoedges.flags);
    if (infoedges.format != ANDROID_BITMAP_FORMAT_A_8) {
        LOGE("Bitmap format is not A_8 !");
        return;
    }

    

    if ((ret = AndroidBitmap_lockPixels(env, bitmapgray, &pixelsgray)) < 0) {
        LOGE(
		"AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }

    if ((ret = AndroidBitmap_lockPixels(env, bitmapedges, &pixelsedge)) < 0) {
        LOGE(
		"AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }

    
    // modify pixels with image processing algorithm
    
    
    LOGI(
		"time to modify pixels....")
;
    
    graydata = (uint8_t *) pixelsgray;
    edgedata = (uint8_t *) pixelsedge;
    
    for (y=0;y<=infogray.height - 1;y++) {
    	for (x=0;x<infogray.width -1;x++) {
    		sumX = 0;
    		sumY = 0;
    		// check boundaries
    		if (y==0 || y == infogray.height-1) {
    			sum = 0;
    		} else if (x == 0 || x == infogray.width -1) {
    			sum = 0;
    		} else {
    			// calc X gradient
    			for (i=-1;i<=1;i++) {
    				for (j=-1;j<=1;j++) {
    					sumX += (int) ( (*(graydata + x + i + (y + j) * infogray.stride)) * Gx[i+1][j+1]);
    				}
    			}
    			
    			// calc Y gradient
    			for (i=-1;i<=1;i++) {
    				for (j=-1;j<=1;j++) {
    					sumY += (int) ( (*(graydata + x + i + (y + j) * infogray.stride)) * Gy[i+1][j+1]);
    				}
    			}
    			
    			sum = abs(sumX) + abs(sumY);
    			
    		}
    		
    		if (sum>255) sum = 255;
    		if (sum<0) sum = 0;
    		
    		*(edgedata + x + y*infogray.width) = 255 - (uint8_t) sum;
    		
    		
    		
    	}
    }
    
    AndroidBitmap_unlockPixels(env, bitmapgray);
    AndroidBitmap_unlockPixels(env, bitmapedges);

    
}

/*
changeBrightness
Pixel Operation
*/
JNIEXPORT void JNICALL Java_com_msi_ibm_ndk_CameraSurfaceView_changeBrightness(JNIEnv * env, jobject  obj, int direction,jobject bitmap)
{
    AndroidBitmapInfo  infogray;
    void*              pixelsgray;
    int                ret;
    int 			y;
    int             x;
    uint8_t save;

    
    
    
    if ((ret = AndroidBitmap_getInfo(env, bitmap, &infogray)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return;
    }

    
    LOGI(
		"gray image :: width is %d; height is %d; stride is %d; format is %d;flags is %d",
		infogray.width, infogray.height, infogray.stride, infogray.format,
		infogray.flags);
    if (infogray.format != ANDROID_BITMAP_FORMAT_A_8) {
        LOGE("Bitmap format is not A_8 !");
        return;
    }
  
    
    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixelsgray)) < 0) {
        LOGE(
		"AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }

    // modify pixels with image processing algorithm
    
    
    LOGI(
		"time to modify pixels....")
;
for (y=0;y<infogray.height;y++) {
	uint8_t * grayline = (uint8_t *) pixelsgray;
	int v;
	for (x=0;x<infogray.width;x++) {
		v = (int) grayline[x];

		if (direction == 1)
		v -=5;
		else
		v += 5;
		if (v >= 255) {
			grayline[x] = 255;
		} else if (v <= 0) {
			grayline[x] = 0;
		} else {
			grayline[x] = (uint8_t) v;
		}
	}

	pixelsgray = (char *) pixelsgray + infogray.stride;
}

AndroidBitmap_unlockPixels(env, bitmap);

}

int* rgbData;
int rgbDataSize = 0;

JNIEXPORT void JNICALL Java_com_msi_ibm_ndk_CameraSurfaceView_YUVtoRBG(JNIEnv * env, jobject obj, jintArray rgb, jbyteArray yuv420sp, jint width, jint height)
{
int sz;
int i;
int j;
int Y;
int Cr = 0;
int Cb = 0;
int pixPtr = 0;
int jDiv2 = 0;
int R = 0;
int G = 0;
int B = 0;
int cOff;
int w = width;
int h = height;
sz = w * h;

jboolean isCopy;
jbyte* yuv = (*env)->GetByteArrayElements(env, yuv420sp, &isCopy);
if(rgbDataSize < sz) {
	int tmp[sz];
	rgbData = &tmp[0];
	rgbDataSize = sz;

}

for(j = 0; j < h; j++) {
	pixPtr = j * w;
	jDiv2 = j >> 1;
	for(i = 0; i < w; i++) {
		Y = yuv[pixPtr];
		if(Y < 0) Y += 255;
		if((i & 0x1) != 1) {
			cOff = sz + jDiv2 * w + (i >> 1) * 2;
			Cb = yuv[cOff];
			if(Cb < 0) Cb += 107; else Cb -= 108;
			Cr = yuv[cOff + 1];
			if(Cr < 0) Cr += 107; else Cr -= 108;
		}
		R = Y + Cr + (Cr >> 2) + (Cr >> 3) + (Cr >> 5); //1.406*~1.403
		if(R < 0) R = 0; else if(R > 255) R = 255;
		G = Y - (Cb >> 2) + (Cb >> 4) + (Cb >> 5) - (Cr >> 1) + (Cr >> 3) + (Cr >> 4) + (Cr >> 5);//
		if(G < 0) G = 0; else if(G > 255) G = 255;
		B = Y + Cb + (Cb >> 1) + (Cb >> 2) + (Cb >> 6);//1.765~1.770
		if(B < 0) B = 0; else if(B > 255) B = 255;
		rgbData[pixPtr++] = 0xff000000 + (B << 16) + (G << 8) + R;
	}
}
(*env)->SetIntArrayRegion(env, rgb, 0, sz, ( jint * ) &rgbData[0] );

(*env)->ReleaseByteArrayElements(env, yuv420sp, yuv, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_com_msi_ibm_ndk_CameraSurfaceView_YUVtoRBGTest(JNIEnv * env, jobject obj, jintArray rgb, jbyteArray yuv420sp, jint width, jint height)
{
int sz;
int i;
int j;
int Y;
int Cr = 0;
int Cb = 0;
int pixPtr = 0;
int jDiv2 = 0;
int R = 0;
int G = 0;
int B = 0;
int cOff;
int w = width;
int h = height;
sz = w * h;

jboolean isCopy;
jbyte* yuv = (*env)->GetByteArrayElements(env, yuv420sp, &isCopy);
//jbyte* yuv = yuv420sp;
if(rgbDataSize < sz) {
	int tmp[sz];
	rgbData = &tmp[0];
	rgbDataSize = sz;
}
//rgbData[0] = 0;
for(j = 0; j < h; j++) {
	pixPtr = j * w;
	jDiv2 = j >> 1;
	for(i = 0; i < w; i++) {
		Y = yuv[pixPtr];
		if(Y < 0) Y += 255;
		if((i & 0x1) != 1) {
			cOff = sz + jDiv2 * w + (i >> 1) * 2;
			Cb = yuv[cOff];
			if(Cb < 0) Cb += 107; else Cb -= 108;
			Cr = yuv[cOff + 1];
			if(Cr < 0) Cr += 107; else Cr -= 108;
		}
		R = Y + Cr + (Cr >> 2) + (Cr >> 3) + (Cr >> 5); //1.406*~1.403
		if(R < 0) R = 0; else if(R > 255) R = 255;
		G = Y - (Cb >> 2) + (Cb >> 4) + (Cb >> 5) - (Cr >> 1) + (Cr >> 3) + (Cr >> 4) + (Cr >> 5);//
		if(G < 0) G = 0; else if(G > 255) G = 255;
		B = Y + Cb + (Cb >> 1) + (Cb >> 2) + (Cb >> 6);//1.765~1.770
		if(B < 0) B = 0; else if(B > 255) B = 255;
		rgbData[pixPtr++] = 0xff000000 + (B << 16) + (G << 8) + R;
//		rgbData[pixPtr++] = (B << 16) + (G << 8) + R;
//		temp[pixPtr++] = 0xff000000 + (B << 16) + (G << 8) + R;
//		*(temp + pixPtr++) = 0xff000000 + (B << 16) + (G << 8) + R;
	}
}
//(*env)->SetIntArrayRegion(env, rgb, 0, sz, ( jint * ) &rgbData[0] );
//
(*env)->ReleaseByteArrayElements(env, yuv420sp, yuv, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_com_msi_ibm_ndk_MainActivity_YUVtoRBG(JNIEnv * env, jobject obj, jintArray rgb, jbyteArray yuv420sp, jint width, jint height)
{
int sz;
int i;
int j;
int Y;
int Cr = 0;
int Cb = 0;
int pixPtr = 0;
int jDiv2 = 0;
int R = 0;
int G = 0;
int B = 0;
int cOff;
int w = width;
int h = height;
sz = w * h;

jboolean isCopy;
jbyte* yuv = (*env)->GetByteArrayElements(env, yuv420sp, &isCopy);
if(rgbDataSize < sz) {
	int tmp[sz];
	rgbData = &tmp[0];
	rgbDataSize = sz;

}

for(j = 0; j < h; j++) {
	pixPtr = j * w;
	jDiv2 = j >> 1;
	for(i = 0; i < w; i++) {
		Y = yuv[pixPtr];
		if(Y < 0) Y += 255;
		if((i & 0x1) != 1) {
			cOff = sz + jDiv2 * w + (i >> 1) * 2;
			Cb = yuv[cOff];
			if(Cb < 0) Cb += 107; else Cb -= 108;
			Cr = yuv[cOff + 1];
			if(Cr < 0) Cr += 107; else Cr -= 108;
		}
		R = Y + Cr + (Cr >> 2) + (Cr >> 3) + (Cr >> 5); //1.406*~1.403
		if(R < 0) R = 0; else if(R > 255) R = 255;
		G = Y - (Cb >> 2) + (Cb >> 4) + (Cb >> 5) - (Cr >> 1) + (Cr >> 3) + (Cr >> 4) + (Cr >> 5);//
		if(G < 0) G = 0; else if(G > 255) G = 255;
		B = Y + Cb + (Cb >> 1) + (Cb >> 2) + (Cb >> 6);//1.765~1.770
		if(B < 0) B = 0; else if(B > 255) B = 255;
		rgbData[pixPtr++] = 0xff000000 + (B << 16) + (G << 8) + R;
	}
}
(*env)->SetIntArrayRegion(env, rgb, 0, sz, ( jint * ) &rgbData[0] );

(*env)->ReleaseByteArrayElements(env, yuv420sp, yuv, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_com_msi_ibm_ndk_CameraSurfaceView_writeArray(JNIEnv * env, jobject obj, jbyteArray rgb, jbyteArray yuv420sp, jint width, jint height)
{
//	jboolean isCopy;
//	jbyte* yuv = (*env)->GetByteArrayElements(env, inArr, &isCopy);
//	(*env)->SetByteArrayRegion(env, outArr, 0, width * height, yuv);
//	(*env)->ReleaseByteArrayElements(env, inArr, yuv, JNI_ABORT);

//	memcpy()

//	jbyte* data = (*env)->GetByteArrayElements(env, yuv420sp, NULL);
//	if (data != NULL) {
//	    memcpy(rgb, data, width * height);
//	    (*env)->ReleaseByteArrayElements(env, yuv420sp, data, JNI_ABORT);
//	}

	int sz;
	int i;
	int j;
	int Y;
	int Cr = 0;
	int Cb = 0;
	int pixPtr = 0;
	int jDiv2 = 0;
	int R = 0;
	int G = 0;
	int B = 0;
	int cOff;
	int w = width;
	int h = height;
	sz = w * h;

	jboolean isCopy;
	jbyte* yuv = (*env)->GetByteArrayElements(env, yuv420sp, &isCopy);
	jbyte* rgbData2;
	if(rgbDataSize < sz) {
		int tmp[sz];
		rgbData2 = &tmp[0];
		rgbDataSize = sz;

	}

	for(j = 0; j < h; j++) {
		pixPtr = j * w;
		jDiv2 = j >> 1;
		for(i = 0; i < w; i++) {
//			Y = yuv[pixPtr];
//			if(Y < 0) Y += 255;
//			if((i & 0x1) != 1) {
//				cOff = sz + jDiv2 * w + (i >> 1) * 2;
//				Cb = yuv[cOff];
//				if(Cb < 0) Cb += 107; else Cb -= 108;
//				Cr = yuv[cOff + 1];
//				if(Cr < 0) Cr += 107; else Cr -= 108;
//			}
//			R = Y + Cr + (Cr >> 2) + (Cr >> 3) + (Cr >> 5); //1.406*~1.403
//			if(R < 0) R = 0; else if(R > 255) R = 255;
//			G = Y - (Cb >> 2) + (Cb >> 4) + (Cb >> 5) - (Cr >> 1) + (Cr >> 3) + (Cr >> 4) + (Cr >> 5);//
//			if(G < 0) G = 0; else if(G > 255) G = 255;
//			B = Y + Cb + (Cb >> 1) + (Cb >> 2) + (Cb >> 6);//1.765~1.770
//			if(B < 0) B = 0; else if(B > 255) B = 255;
//			rgbData[pixPtr++] = 0xff000000 + (B << 16) + (G << 8) + R;
			rgbData2[pixPtr++] = yuv[pixPtr];
		}
	}
	(*env)->SetByteArrayRegion(env, rgb, 0, sz, ( jbyte * ) &rgbData2[0] );

	(*env)->ReleaseByteArrayElements(env, yuv420sp, yuv, JNI_ABORT);
}

JNIEXPORT jstring JNICALL Java_com_msi_ibm_ndk_MainActivity_messageFromNativeCode(
	JNIEnv * env, jobject obj) {
return (*env)->NewStringUTF(env, "Hello MainActivity!");
}

JNIEXPORT jstring JNICALL Java_com_msi_ibm_ndk_CameraSurfaceView_messageFromNativeCode(
	JNIEnv * env, jobject obj) {
return (*env)->NewStringUTF(env, "Hello CameraSurfaceView!");
}

JavaVM* g_jvm = 0;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void * reserved)
{
    g_jvm = vm;
    return JNI_VERSION_1_6;
}

void JNI_OnUnload(JavaVM *vm, void *reserved)
{
    g_jvm = 0;
}

JNIEXPORT jstring JNICALL Java_com_msi_ibm_ndk_MainActivity_Test(JNIEnv * env, jobject obj)
{
	int res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);
	if (res == 0) {
//		(*g_jvm)->DetachCurrentThread(g_jvm);
		return (*env)->NewStringUTF(env, "0");
	}
	else {
//		(*g_jvm)->DetachCurrentThread(g_jvm);
		return (*env)->NewStringUTF(env, "nop");
	}
}

JNIEXPORT void JNICALL Java_com_msi_ibm_ndk_MainActivity_Test2(JNIEnv * env, jobject obj, jintArray rgb, jbyteArray yuv420sp, jint width, jint height)
{
int res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);
if (res == 0) {
	int sz;
	int i;
	int j;
	int Y;
	int Cr = 0;
	int Cb = 0;
	int pixPtr = 0;
	int jDiv2 = 0;
	int R = 0;
	int G = 0;
	int B = 0;
	int cOff;
	int w = width;
	int h = height;
	sz = w * h;

	jboolean isCopy;
	jbyte* yuv = (*env)->GetByteArrayElements(env, yuv420sp, &isCopy);
	if(rgbDataSize < sz) {
		int tmp[sz];
		rgbData = &tmp[0];
		rgbDataSize = sz;

	}

	for(j = 0; j < h; j++) {
		pixPtr = j * w;
		jDiv2 = j >> 1;
		for(i = 0; i < w; i++) {
			Y = yuv[pixPtr];
			if(Y < 0) Y += 255;
			if((i & 0x1) != 1) {
				cOff = sz + jDiv2 * w + (i >> 1) * 2;
				Cb = yuv[cOff];
				if(Cb < 0) Cb += 107; else Cb -= 108;
				Cr = yuv[cOff + 1];
				if(Cr < 0) Cr += 107; else Cr -= 108;
			}
			R = Y + Cr + (Cr >> 2) + (Cr >> 3) + (Cr >> 5); //1.406*~1.403
			if(R < 0) R = 0; else if(R > 255) R = 255;
			G = Y - (Cb >> 2) + (Cb >> 4) + (Cb >> 5) - (Cr >> 1) + (Cr >> 3) + (Cr >> 4) + (Cr >> 5);//
			if(G < 0) G = 0; else if(G > 255) G = 255;
			B = Y + Cb + (Cb >> 1) + (Cb >> 2) + (Cb >> 6);//1.765~1.770
			if(B < 0) B = 0; else if(B > 255) B = 255;
			rgbData[pixPtr++] = 0xff000000 + (B << 16) + (G << 8) + R;
			pixPtr++;
		}
	}
	(*env)->SetIntArrayRegion(env, rgb, 0, sz, ( jint * ) &rgbData[0] );

	(*env)->ReleaseByteArrayElements(env, yuv420sp, yuv, JNI_ABORT);
}
}

JNIEXPORT void JNICALL Java_com_msi_ibm_ndk_CameraSurfaceView_Test2(JNIEnv * env, jobject obj, jintArray rgb, jbyteArray yuv420sp, jint width, jint height)
{
int res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);
if (res == 0) {
	int sz;
	int i;
	int j;
	int Y;
	int Cr = 0;
	int Cb = 0;
	int pixPtr = 0;
	int jDiv2 = 0;
	int R = 0;
	int G = 0;
	int B = 0;
	int cOff;
	int w = width;
	int h = height;
	sz = w * h;

	jboolean isCopy;
	jbyte* yuv = (*env)->GetByteArrayElements(env, yuv420sp, &isCopy);
	if(rgbDataSize < sz) {
		int tmp[sz];
		rgbData = &tmp[0];
		rgbDataSize = sz;

	}

	for(j = 0; j < h; j++) {
		pixPtr = j * w;
		jDiv2 = j >> 1;
		for(i = 0; i < w; i++) {
			Y = yuv[pixPtr];
			if(Y < 0) Y += 255;
			if((i & 0x1) != 1) {
				cOff = sz + jDiv2 * w + (i >> 1) * 2;
				Cb = yuv[cOff];
				if(Cb < 0) Cb += 107; else Cb -= 108;
				Cr = yuv[cOff + 1];
				if(Cr < 0) Cr += 107; else Cr -= 108;
			}
			R = Y + Cr + (Cr >> 2) + (Cr >> 3) + (Cr >> 5); //1.406*~1.403
			if(R < 0) R = 0; else if(R > 255) R = 255;
			G = Y - (Cb >> 2) + (Cb >> 4) + (Cb >> 5) - (Cr >> 1) + (Cr >> 3) + (Cr >> 4) + (Cr >> 5);//
			if(G < 0) G = 0; else if(G > 255) G = 255;
			B = Y + Cb + (Cb >> 1) + (Cb >> 2) + (Cb >> 6);//1.765~1.770
			if(B < 0) B = 0; else if(B > 255) B = 255;
			rgbData[pixPtr++] = 0xff000000 + (B << 16) + (G << 8) + R;
			pixPtr++;
		}
	}
	(*env)->SetIntArrayRegion(env, rgb, 0, sz, ( jint * ) &rgbData[0] );

	(*env)->ReleaseByteArrayElements(env, yuv420sp, yuv, JNI_ABORT);
}
}
