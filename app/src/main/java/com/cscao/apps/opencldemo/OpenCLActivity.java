//
//  OpenCLActivity.java
//  OpenCL Example1
//
//  Created by Rasmusson, Jim on 18/03/13.
//
//  Copyright (c) 2013, Sony Mobile Communications AB
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//
//     * Neither the name of Sony Mobile Communications AB nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
//  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//  DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
//  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 

package com.cscao.apps.opencldemo;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Locale;

public class OpenCLActivity extends Activity {
    protected static final String TAG = "OpenCLActivity";

    private void copyFile(final String f) {
        InputStream in;
        try {
            in = getAssets().open(f);
            final File of = new File(getDir("execdir", MODE_PRIVATE), f);

            final OutputStream out = new FileOutputStream(of);

            final byte b[] = new byte[65535];
            int sz = 0;
            while ((sz = in.read(b)) > 0) {
                out.write(b, 0, sz);
            }
            in.close();
            out.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    static boolean hasFoundLibrary = true;

    static {
        try {
            System.loadLibrary("OpenCLDemo");
        } catch (UnsatisfiedLinkError e) {
            hasFoundLibrary = false;
        }
    }

    public static native int runOpenCL(Bitmap bmpIn, Bitmap bmpOut, int info[]);

    public static native int runNativeC(Bitmap bmpIn, Bitmap bmpOut, int info[]);

    final int info[] = new int[3]; // Width, Height, Execution time (ms)

    Bitmap bmpOrig, bmpOpenCL, bmpNativeC;
    ImageView imageView;
    TextView textView;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        imageView = findViewById(R.id.imageHere);
        textView = findViewById(R.id.resultText);

        copyFile("bilateralKernel.cl"); //copy cl kernel file from assets to /data/data/...assets

        bmpOrig = BitmapFactory.decodeResource(this.getResources(), R.drawable.brusigablommor);
        info[0] = bmpOrig.getWidth();
        info[1] = bmpOrig.getHeight();

        bmpOpenCL = Bitmap.createBitmap(info[0], info[1], Config.ARGB_8888);
        bmpNativeC = Bitmap.createBitmap(info[0], info[1], Config.ARGB_8888);
        textView.setText(R.string.original_str);
        imageView.setImageBitmap(bmpOrig);
    }

    public void showOriginalImage(View v) {
        textView.setText(R.string.original_str);
        imageView.setImageBitmap(bmpOrig);
    }

    public void showOpenCLImage(View v) {
        info[2] = 0;
        runOpenCL(bmpOrig, bmpOpenCL, info);
        textView.setText(String.format(Locale.US, "%s %d ms",
                getString(R.string.opencl_time_str), info[2]));
        imageView.setImageBitmap(bmpOpenCL);
    }

    public void showNativeCImage(View v) {
        info[2] = 0;
        runNativeC(bmpOrig, bmpNativeC, info);
        textView.setText(String.format(Locale.US, "%s %d ms",
                getString(R.string.native_time_str), info[2]));
        imageView.setImageBitmap(bmpNativeC);
    }
}
