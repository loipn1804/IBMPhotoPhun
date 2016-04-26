package com.msi.ibm.ndk;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.Toast;

public class MainActivity extends Activity {

	private FrameLayout frameCamera;

	private Bitmap bitmapOrig = null;
	private Bitmap bitmapGray = null;
	private Bitmap bitmapWip = null;

	private CameraSurfaceView cameraSurfaceView;

	private ImageView ivDisplay = null;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		ivDisplay = (ImageView) findViewById(R.id.imageView);

		frameCamera = (FrameLayout) findViewById(R.id.frameCamera);
		frameCamera.addView(new CameraSurfaceView(this, ivDisplay, myCallback));

		// // load bitmap from resources
		// BitmapFactory.Options options = new BitmapFactory.Options();
		// // Make sure it is 24 bit color as our image processing algorithm
		// expects this format
		// options.inPreferredConfig = Config.ARGB_8888;
		// bitmapOrig = BitmapFactory.decodeResource(this.getResources(),
		// R.drawable.sampleimage,options);
		// if (bitmapOrig != null) {
		// bitmapWip =
		// Bitmap.createBitmap(bitmapOrig.getWidth(),bitmapOrig.getHeight(),Config.ALPHA_8);
		// convertToGray(bitmapOrig,bitmapWip);
		//
		// ivDisplay.setImageBitmap(bitmapWip);
		// }

		

//		Handler handler = new Handler();
//		handler.post(runnable);
		Thread thread = new Thread(runnable);
		thread.start();
	}
	
	private Runnable runnable = new Runnable() {
		
		@Override
		public void run() {
			// TODO Auto-generated method stub
			messageFromNativeCode();
//			Toast.makeText(MainActivity.this, messageFromNativeCode(), Toast.LENGTH_SHORT).show();
//			Toast.makeText(MainActivity.this, "runable thread", Toast.LENGTH_SHORT).show();
		}
	};
	
	private MyCallback myCallback = new MyCallback() {

		@Override
		public void execute(int[] rgb, byte[] yuv, int width, int height) {
			// TODO Auto-generated method stub
			YUVtoRBG(rgb, yuv, width, height);
		}
	};

	// NDK STUFF
	static {
		System.loadLibrary("ibmphotophun");
	}

	public native void YUVtoRBG(int[] rgb, byte[] yuv, int width, int height);

	public native String messageFromNativeCode();

	// END NDK STUFF
}
