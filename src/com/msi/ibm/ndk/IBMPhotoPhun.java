
/*
 * IBMPhotoPhun.java
 * 
 * Author: Frank Ableson
 * Contact Info: fableson@navitend.com
 */

package com.msi.ibm.ndk;



import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.graphics.BitmapFactory;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.view.View;
import android.widget.ImageView;


public class IBMPhotoPhun extends Activity {
	private String tag = "IBMPhotoPhun";
	private Bitmap bitmapOrig = null;
	private Bitmap bitmapGray = null;
	private Bitmap bitmapWip = null;
	private ImageView ivDisplay = null;
	
	
	
	// NDK STUFF
    static {
    	System.loadLibrary("ibmphotophun");
    }
	public native void convertToGray(Bitmap bitmapIn,Bitmap bitmapOut);
	public native void changeBrightness(int direction,Bitmap bitmap);
	public native void findEdges(Bitmap bitmapIn,Bitmap bitmapOut);
	// END NDK STUFF
	
	
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        Log.i(tag,"before image stuff");
        ivDisplay = (ImageView) findViewById(R.id.ivDisplay);
        
    
        // load bitmap from resources
        BitmapFactory.Options options = new BitmapFactory.Options();
        // Make sure it is 24 bit color as our image processing algorithm expects this format
        options.inPreferredConfig = Config.ARGB_8888;
        bitmapOrig = BitmapFactory.decodeResource(this.getResources(), R.drawable.sampleimage,options);
        if (bitmapOrig != null) {
        	bitmapWip = Bitmap.createBitmap(bitmapOrig.getWidth(),bitmapOrig.getHeight(),Config.ALPHA_8);
        	convertToGray(bitmapOrig,bitmapWip);
        
        	ivDisplay.setImageBitmap(bitmapWip);
        }
      
    }
 
    public void onResetImage(View v) {
    	Log.i(tag,"onResetImage");

    	ivDisplay.setImageBitmap(bitmapOrig);
    	
    }
 
    public void onFindEdges(View v) {
    	Log.i(tag,"onFindEdges");

    	// make sure our target bitmaps are happy
    	bitmapGray = Bitmap.createBitmap(bitmapOrig.getWidth(),bitmapOrig.getHeight(),Config.ALPHA_8);
    	bitmapWip = Bitmap.createBitmap(bitmapOrig.getWidth(),bitmapOrig.getHeight(),Config.ALPHA_8);
    	// before finding edges, we need to convert this image to gray
    	convertToGray(bitmapOrig,bitmapGray);
    	// find edges in the image
    	findEdges(bitmapGray,bitmapWip);
    	ivDisplay.setImageBitmap(bitmapWip);
    	
    }
    public void onConvertToGray(View v) {
    	Log.i(tag,"onConvertToGray");
 
    	bitmapWip = Bitmap.createBitmap(bitmapOrig.getWidth(),bitmapOrig.getHeight(),Config.ALPHA_8);
    	convertToGray(bitmapOrig,bitmapWip);
    	ivDisplay.setImageBitmap(bitmapWip);
    }
    
    public void onDimmer(View v) {
    	Log.i(tag,"onDimmer");
    	
    	changeBrightness(2,bitmapWip);
    	ivDisplay.setImageBitmap(bitmapWip);
    }
    public void onBrighter(View v) {
    	Log.i(tag,"onBrighter");
  
    	changeBrightness(1,bitmapWip);
    	ivDisplay.setImageBitmap(bitmapWip);
    }
     
    
    
}