package com.msi.ibm.ndk;

import java.io.ByteArrayOutputStream;
import java.util.List;

import com.msi.ibm.ndk.MainActivity.MyThread;

import thread.UIThreadExecutor;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.ImageFormat;
import android.graphics.Matrix;
import android.graphics.Rect;
import android.graphics.YuvImage;
import android.graphics.Bitmap.Config;
import android.hardware.Camera;
import android.hardware.Camera.PreviewCallback;
import android.hardware.Camera.Size;
import android.os.AsyncTask;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.Toast;

public class CameraSurfaceView extends SurfaceView implements
		SurfaceHolder.Callback {

	private ImageView imageView;

	private Activity context;
	private Camera camera;
	private SurfaceHolder surfaceHolder;
	private MyCallback myCallback;

	private HandlerThread mCameraThread = null;
	private Handler mCameraHandler = null;
	private Thread mThread = null;

	private int degrees = 0;

	private long startTime = System.currentTimeMillis();

	private int imageFormat;

	private boolean bProcessing = false;
	private int[] pixels = null;
	private byte[] FrameData = null;
	private Bitmap bitmap = null;
	private int PreviewSizeWidth;
	private int PreviewSizeHeight;

	int width = 1024;
	int height = 768;

	int[] rgba;
	Bitmap bmp;
	byte[] data_temp;

	boolean isHaveFrame = false;

	String sTemp = "";

	Handler mHandler = new Handler(Looper.getMainLooper());

	MyThread myThread;
	boolean stop;
	byte[] in;
	int[] out;

	// NDK STUFF
	static {
		System.loadLibrary("ibmphotophun");
	}

	public native void convertToGray(Bitmap bitmapIn, Bitmap bitmapOut);

	public native void changeBrightness(int direction, Bitmap bitmap);

	public native void findEdges(Bitmap bitmapIn, Bitmap bitmapOut);

	public native void YUVtoRBG(int[] rgb, byte[] yuv, int width, int height);

	public native void YUVtoRBGTest(int[] rgb, byte[] yuv, int width, int height);

	public native void writeArray(byte[] in, byte[] out, int width, int height);

	public native void Test2(int[] rgb, byte[] yuv, int width, int height);

	public native String messageFromNativeCode();

	// END NDK STUFF

	public CameraSurfaceView(Activity context, ImageView imageView,
			MyCallback myCallback) {
		super(context);
		this.context = context;
		this.imageView = imageView;
		this.myCallback = myCallback;
		surfaceHolder = this.getHolder();
		surfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
		// surfaceHolder.setFixedSize(768, 1024);

		setWillNotDraw(false);

		// DisplayMetrics metrics = new DisplayMetrics();
		// ((Activity) context).getWindowManager().getDefaultDisplay()
		// .getMetrics(metrics);
		// Display display = ((WindowManager) context
		// .getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();

		rgba = new int[width * height];
		data_temp = new byte[width * height];
		bmp = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);

//		 startCameraWithThread();
		startCameraWithoutThread();
	}

	private void startCameraWithThread() {
		if (mCameraThread == null) {
			mCameraThread = new HandlerThread("CAMERA_THREAD_NAME",
					Thread.MAX_PRIORITY);
			mCameraThread.start();
			mCameraHandler = new Handler(mCameraThread.getLooper());
		}
		mCameraHandler.post(new Runnable() {
			@Override
			public void run() {
				try {
					// open the camera
					camera = Camera.open();
				} catch (RuntimeException e) {
					// check for exceptions
					// System.err.println(e);
					return;
				}

				// int rotation = ((WindowManager) context
				// .getSystemService(Context.WINDOW_SERVICE))
				// .getDefaultDisplay().getRotation();
				// degrees = 0;
				// switch (rotation) {
				// case Surface.ROTATION_0:
				// degrees = 90;
				// break;
				// case Surface.ROTATION_90:
				// degrees = 0;
				// break;
				// case Surface.ROTATION_180:
				// degrees = 270;
				// break;
				// case Surface.ROTATION_270:
				// degrees = 180;
				// break;
				// }
				// camera.setDisplayOrientation(degrees);

				try {
					camera.setPreviewDisplay(surfaceHolder);
					camera.setPreviewCallback(previewCallback);
					camera.startPreview();
				} catch (Exception e) {
					// check for exceptions
					// System.err.println(e);
					return;
				}
			}
		});

		surfaceHolder.addCallback(this);
		
		in = new byte[4];
		in[0] = 1;
		in[1] = 2;
		in[2] = 3;
		in[3] = 4;
		out = new int[4];
		stop = false;
		myThread = new MyThread();
		myThread.start();
	}

	private void startCameraWithoutThread() {
		try {
			// open the camera
			camera = Camera.open();
		} catch (RuntimeException e) {
			// check for exceptions
			// System.err.println(e);
			return;
		}

		// int rotation = ((WindowManager) context
		// .getSystemService(Context.WINDOW_SERVICE))
		// .getDefaultDisplay().getRotation();
		// degrees = 0;
		// switch (rotation) {
		// case Surface.ROTATION_0:
		// degrees = 90;
		// break;
		// case Surface.ROTATION_90:
		// degrees = 0;
		// break;
		// case Surface.ROTATION_180:
		// degrees = 270;
		// break;
		// case Surface.ROTATION_270:
		// degrees = 180;
		// break;
		// }
		// camera.setDisplayOrientation(degrees);

		try {
			camera.setPreviewDisplay(surfaceHolder);
			camera.setPreviewCallback(previewCallback);
			camera.startPreview();
		} catch (Exception e) {
			// check for exceptions
			// System.err.println(e);
			return;
		}

		surfaceHolder.addCallback(this);

		// mThread = new Thread(new CameraWorker());
		// mThread.start();

//		in = new byte[4];
//		in[0] = 1;
//		in[1] = 2;
//		in[2] = 3;
//		in[3] = 4;
//		out = new int[4];
//		stop = false;
//		myThread = new MyThread();
//		myThread.start();
		// Test2(out, in, 2, 2);
		// String msg = "";
		// for (int i = 0; i < 4; i++) {
		// msg += in[i] + "+" + out[i] + ",";
		// in[i]++;
		// }
		// Log.i("runnable", msg);
	}

	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width,
			int height) {
		// TODO Auto-generated method stub
		if (camera != null) {
			camera.setPreviewCallback(null);
			camera.stopPreview();

			Camera.Parameters param;
			param = camera.getParameters();
			param.setPreviewSize(this.width, this.height);
			// param.setColorEffect("sepia");
			camera.setParameters(param);

			try {
				camera.setPreviewDisplay(surfaceHolder);
				camera.setPreviewCallback(previewCallback);
			} catch (Exception e) {

			}
			camera.startPreview();
		}
	}

	@Override
	public void surfaceCreated(SurfaceHolder holder) {
		// TODO Auto-generated method stub

	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
		// TODO Auto-generated method stub
		camera.stopPreview();
		camera.setPreviewCallback(null);
		camera.release();
		camera = null;

		// try {
		// mThread.join();
		// } catch (InterruptedException e) {
		// // TODO Auto-generated catch block
		// e.printStackTrace();
		// }
		// mThread = null;

		stop = true;
		if (myThread != null) {
			myThread.interrupt();
			myThread = null;
			Log.i("runnable", "Destroy myThread");
		}
	}

	private PreviewCallback previewCallback = new PreviewCallback() {

		@Override
		public void onPreviewFrame(final byte[] data, Camera camera) {
//			if (!isHaveFrame) {
				data_temp = data;
//				// YUVtoRBG(rgba, data_temp, width, height);
//				// bmp.setPixels(rgba, 0/* offset */, width /* stride */, 0, 0,
//				// width,
//				// height);
//				// imageView.setImageBitmap(bmp);
//				// // MyAsyn asyn = new MyAsyn();
//				// // asyn.execute(data);
//				isHaveFrame = true;
//			}
			
			// if (!bProcessing) {
			// data_temp = data;
			// // mHandler.post(DoImageProcessing);
			// bProcessing = true;
			// YUVtoRBG(rgba, data_temp, width, height);
			// bmp.setPixels(rgba, 0/* offset */, width /* stride */, 0, 0,
			// width,
			// height);
			// mHandler.post(DoImageProcessing);
			// bProcessing = false;
			// }
			// final Bitmap bmp = Bitmap.createBitmap(width, height,
			// Bitmap.Config.ARGB_8888);
			// data_temp = data;
			 YUVtoRBG(rgba, data_temp, width, height);
			 bmp.setPixels(rgba, 0/* offset */, width /* stride */, 0, 0,
			 width,
			 height);
//			 imageView.setImageBitmap(bmp);
			
			 long now = System.currentTimeMillis();
			 Log.i("onPreviewFrame", "run-" + (now - startTime));
			 startTime = now;

			// UIThreadExecutor.getInstance().runOnUIThread(new Runnable() {
			// @Override
			// public void run() {
			// // Bitmap bmp = Bitmap.createBitmap(width, height,
			// // Bitmap.Config.ARGB_8888);
			// YUVtoRBG(rgba, data, width, height);
			// bmp.setPixels(rgba, 0/* offset */, width /* stride */, 0, 0,
			// width, height);
			// imageView.setImageBitmap(bmp);
			//
			// long now = System.currentTimeMillis();
			// Log.i("onPreviewFrame", "run-" + (now - startTime));
			// startTime = now;
			// }
			// });

			// YUVtoRBG(rgba, data, width, height);
			// bmp.setPixels(rgba, 0/* offset */, width /* stride */, 0, 0,
			// width,
			// height);
			// Canvas canvas = surfaceHolder.lockCanvas();
			// if (canvas != null) {
			// canvas.drawBitmap(bmp,
			// (canvas.getWidth() - width) / 2,
			// (canvas.getHeight() - height) / 2, null);
			// surfaceHolder.unlockCanvasAndPost(canvas);
			// Log.w("canvas", "Canvas is have!");
			// } else {
			// Log.w("canvas", "Canvas is null!");
			// }

			// myCallback.execute(rgba, data, width, height);
		}
	};

	private class CameraWorker implements Runnable {

		@Override
		public synchronized void run() {
			final String abc = messageFromNativeCode();

			UIThreadExecutor.getInstance().runOnUIThread(new Runnable() {
				@Override
				public void run() {
					Toast.makeText(context, abc, Toast.LENGTH_SHORT).show();
				}
			});
			boolean isRunAlready = false;
			while (true) {
				try {
					Thread.sleep(1000);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				if (isHaveFrame && !isRunAlready) {
					YUVtoRBGTest(rgba, data_temp, width, height);
					// isHaveFrame = false;
					isRunAlready = true;
				}
			}
		}
	}

	private class MyAsyn extends AsyncTask<byte[], Void, Void> {

		@Override
		protected synchronized Void doInBackground(byte[]... arg0) {
			byte[] data = arg0[0];
			YUVtoRBGTest(rgba, data, width, height);
			return null;
		}
	}

	private Runnable DoImageProcessing = new Runnable() {
		public void run() {
			// bProcessing = true;
			// YUVtoRBG(rgba, data_temp, width, height);
			// bmp.setPixels(rgba, 0/* offset */, width /* stride */, 0, 0,
			// width,
			// height);
			imageView.setImageBitmap(bmp);
			// bProcessing = false;

			long now = System.currentTimeMillis();
			Log.i("DoImageProcessing", "run-" + (now - startTime));
			startTime = now;
		}
	};

	class MyThread extends Thread {
		@Override
		public void run() {
			// TODO Auto-generated method stub
			super.run();
			while (!stop) {
//				try {
//					Thread.sleep(1000);
//				} catch (InterruptedException e) {
//					// TODO Auto-generated catch block
//					e.printStackTrace();
//				}
				if (stop) {
					return;
				}
				if (isHaveFrame) {
					// Test2(out, in, 2, 2);
					// YUVtoRBG(out, in, 2, 2);
					YUVtoRBG(rgba, data_temp, width, height);
					bmp.setPixels(rgba, 0/* offset */, width /* stride */, 0, 0,
							width, height);

					// imageView.setImageBitmap(bmp);
					String msg = "";
					for (int i = 0; i < 4; i++) {
						msg += in[i] + "+" + rgba[i] + ",";
						in[i]++;
					}
					Log.i("runnable", msg);
					// toast.setText(in[0] + "");
					// toast.show();
					UIThreadExecutor.getInstance().runOnUIThread(
							new Runnable() {
								@Override
								public void run() {
									imageView.setImageBitmap(bmp);
									 long now = System.currentTimeMillis();
									 Log.i("onPreviewFrame", "run-" + (now - startTime));
									 startTime = now;
									isHaveFrame = false;
								}
							});
				}
			}
		}
	}
}
