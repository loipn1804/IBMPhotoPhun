package com.msi.ibm.ndk;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
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

	byte[] in;
	int[] out;

	Thread thread;
	MyThread myThread;

	LooperThread looperThread;
	boolean stop;
	
	Toast toast;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		ivDisplay = (ImageView) findViewById(R.id.imageView);

		 frameCamera = (FrameLayout) findViewById(R.id.frameCamera);
		 frameCamera.addView(new CameraSurfaceView(this, ivDisplay,
		 myCallback));

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
		
		toast = Toast.makeText(this, "", Toast.LENGTH_SHORT);

		in = new byte[4];
		in[0] = 1;
		in[1] = 2;
		in[2] = 3;
		in[3] = 4;
		out = new int[4];
		
		stop = false;
		// Handler handler = new Handler();
		// handler.post(runnable);
//		thread = new Thread(runnable);
//		thread.start();
//		myThread = new MyThread();
//		myThread.start();
//		 looperThread = new LooperThread();
//		 looperThread.start();

		// Test2(out, in, 2, 2);
		// String msg = "";
		// for (int i = 0; i < 4; i++) {
		// msg += out[i] + ".";
		// }
		// Log.i("runnable", msg);

		// YUVtoRBG(out, in, 2, 2);

		// Toast.makeText(this, Test(), Toast.LENGTH_LONG).show();
		 
//		 new Handler().postDelayed(new Runnable() {
//			
//			@Override
//			public void run() {
//				// TODO Auto-generated method stub
//				looperThread.notify();
//			}
//		}, 5000);
	}

	private Runnable runnable = new Runnable() {

		@Override
		public void run() {
			while (!stop) {
//				Test2(out, in, 2, 2);
				String msg = "";
				for (int i = 0; i < 4; i++) {
					msg += in[i] + "+" + out[i] + ",";
				}
				Log.i("runnable", msg);
			}
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

	public native void Test2(int[] rgb, byte[] yuv, int width, int height);

	public native String Test();

	// END NDK STUFF

	@Override
	protected void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
		stop = true;
		if (thread != null) {
			thread.interrupt();
			thread = null;
			Log.i("runnable", "Destroy thread");
		}
		if (myThread != null) {
			myThread.interrupt();
			myThread = null;
			Log.i("runnable", "Destroy myThread");
		}
		if (looperThread != null) {
			looperThread.interrupt();
			looperThread = null;
			Log.i("runnable", "Destroy looperThread");
		}
	}

	class LooperThread extends Thread {
		public Handler mHandler;

		@Override
		public void run() {
			super.run();
			while (!stop) {
				Looper.prepare();

				mHandler = new Handler() {
					public void handleMessage(Message msg) {
						// process incoming messages here
					}
				};
				mHandler.post(new Runnable() {
					
					@Override
					public void run() {
						// TODO Auto-generated method stub
						Log.i("runnable", "post");
						Looper.myLooper().quit();
					}
				});

				try {
					sleep(1000);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				if (stop) {
					return;
				}

//				Test2(out, in, 2, 2);
				String msg = "";
				for (int i = 0; i < 4; i++) {
					msg += out[i] + ",";
				}
				Log.i("runnable", msg);

				Looper.loop();
			}
		}
	}
	
	class MyThread extends Thread {
		@Override
		public void run() {
			// TODO Auto-generated method stub
			super.run();
			while (!stop) {
				try {
					Thread.sleep(1000);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				if (stop) {
					return;
				}
				Test2(out, in, 2, 2);
				String msg = "";
				for (int i = 0; i < 4; i++) {
					msg += in[i] + "+" + out[i] + ",";
					in[i]++;
				}
				Log.i("runnable", msg);
//				toast.setText(in[0] + "");
//				toast.show();
			}
		}
	}
}
