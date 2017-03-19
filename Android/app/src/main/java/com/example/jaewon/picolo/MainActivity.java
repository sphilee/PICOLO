package com.example.jaewon.picolo;

import android.Manifest;
import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;
import android.provider.DocumentsContract;
import android.provider.MediaStore;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.Toast;

import android.content.ActivityNotFoundException;
import android.content.CursorLoader;
import android.database.Cursor;
import android.util.Log;


import com.koushikdutta.async.future.FutureCallback;
import com.koushikdutta.async.http.body.FileBody;
import com.koushikdutta.async.http.body.StringBody;
import com.koushikdutta.ion.Ion;
import com.koushikdutta.ion.Response;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.ProtocolException;
import java.net.URL;


public class MainActivity extends AppCompatActivity {

    Button camera = null;
    Button selimg = null;
    Button uploadimg = null;
    ImageView img = null;

    int serverResponseCode = 0;
    String path = " ";
    private static final int PICK_FROM_ALBUM = 1;
    private String up ="http://35.162.178.8:9000/upload/";//"http://121.167.34.47:3000";

   /* String boundary = "*****";
    private String strUrl,strCookie,result;
    String fileName= "upload_image";
    int bytesRead, bytesAvailable, bufferSize;
    byte[] buffer;
    int maxBufferSize = 80;*/

    public class HttpConnectionThread extends AsyncTask<String, Void, String> {
        @Override

        protected String doInBackground(String... url) {
            // URL 연결이 구현될 부분
                uploadFile(path);

            return null;
        }

    }

    private void connectHTTP() {
        HttpConnectionThread task = new HttpConnectionThread();
        task.execute();
        task = null;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        Get_permission();

        Button_setup();

    }

    private void Get_permission() {
        if (ContextCompat.checkSelfPermission(this,
                Manifest.permission.READ_EXTERNAL_STORAGE)
                != PackageManager.PERMISSION_GRANTED) {

            // Should we show an explanation?
            if (ActivityCompat.shouldShowRequestPermissionRationale(this,
                    Manifest.permission.READ_EXTERNAL_STORAGE)) {

            } else {
                ActivityCompat.requestPermissions(this,
                        new String[]{Manifest.permission.READ_EXTERNAL_STORAGE},
                        5555);
            }
        }

    }


    private void Button_setup() {
        camera = (Button) findViewById(R.id.camera);
        selimg = (Button) findViewById(R.id.selimg);
        img = (ImageView) findViewById(R.id.img);
        uploadimg = (Button) findViewById(R.id.uploadimg);

        camera.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
                startActivity(intent);
            }
        });

        uploadimg.setVisibility(View.INVISIBLE);
        uploadimg.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                connectHTTP();
            }
        });

        selimg.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent fintent = new Intent(Intent.ACTION_PICK);
                fintent.setType(android.provider.MediaStore.Images.Media.CONTENT_TYPE);
                try {
                    startActivityForResult(fintent, PICK_FROM_ALBUM);
                } catch (ActivityNotFoundException e) {

                }
            }
        });
    }

    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == 1)
            img.setImageURI(data.getData());
            uploadimg.setVisibility(View.VISIBLE);
            if (resultCode == Activity.RESULT_OK) {
                Uri selectedImage = data.getData();

                path = getPathFromURI(selectedImage);
                String file_extn = path.substring(path.lastIndexOf(".") + 1);


                if (file_extn.equals("img") || file_extn.equals("jpg") || file_extn.equals("jpeg") || file_extn.equals("gif") || file_extn.equals("png")) {
                    //FINE
                } else {
                    //NOT IN REQUIRED FORMAT
                }
            }

    }

    private String getPathFromURI(Uri uri) {
        String[] projection = {MediaStore.Images.Media.DATA};
        CursorLoader loader = new CursorLoader(getApplicationContext(), uri, projection, null, null, null);
        Cursor cursor = loader.loadInBackground();
        int column_index = cursor.getColumnIndexOrThrow(MediaStore.Images.Media.DATA);
        cursor.moveToFirst();
        Log.i(">>>>>>>>>>path: ", cursor.getString(column_index));
        return cursor.getString(column_index);

    }

    public int uploadFile(String sourceFileUri) {
        String attachmentName = "photo";
        String crlf = "\r\n";

        int serverResponseCode = 0;
        String fileName = sourceFileUri;
        HttpURLConnection conn = null;
        DataOutputStream dos = null;
        String lineEnd = "\r\n";
        String twoHyphens = "--";
        String boundary = "*****";
        int bytesRead, bytesAvailable, bufferSize;
        byte[] buffer;
        int maxBufferSize = 80;
        File sourceFile = new File(sourceFileUri);

        if (!sourceFile.isFile()) {



            Log.e("uploadFile", "Source File not exist :"+path);

            runOnUiThread(new Runnable() {
                public void run() {

                }
            });

            return 0;

        }
        else
        {
           try {

                // open a URL connection to the Servlet
               FileInputStream fileInputStream = new FileInputStream(sourceFile);
                URL url = new URL(up);

                // Open a HTTP  connection to  the URL
                conn = (HttpURLConnection) url.openConnection();
               conn.setDoInput(true); // Allow Inputs
               conn.setDoOutput(true); // Allow Outputs
               conn.setUseCaches(false); // Don't use a Cached Copy
               conn.setRequestMethod("POST");
               conn.setRequestProperty("Connection", "Keep-Alive");
               conn.setRequestProperty("fieldname",attachmentName);
                conn.setRequestProperty("Cache-Control", "no-cache");
                conn.setRequestProperty("ENCTYPE", "multipart/form-data");
                conn.setRequestProperty("Content-Type", "multipart/form-data;boundary=" + boundary);
               //conn.setRequestProperty("uploaded_file", fileName);

                dos = new DataOutputStream(conn.getOutputStream());

                dos.writeBytes(twoHyphens + boundary + crlf);
                dos.writeBytes("Content-Disposition: form-data; name=\"" + attachmentName + "\";filename=\"" + path + "\"" + crlf);
                //dos.writeBytes("Content-Disposition: form-data; name=\"" + attachmentName + "\";filename=\"path\"" + crlf);
                dos.writeBytes(crlf);



                // create a buffer of  maximum size
                /*if (path != null) {
                    FileInputStream fileInputStream = new FileInputStream(sourceFileUri);

                    bytesAvailable = fileInputStream.available();
                    bufferSize = Math.min(bytesAvailable, maxBufferSize);
                    buffer = new byte[bufferSize];

                    int res = 1;
                    while ((res = fileInputStream.read(buffer)) > 0) {
                        OutputStream os = conn.getOutputStream();
                        os.write(buffer, 0, res);
                    }
                }*/
               bytesAvailable = fileInputStream.available();

               bufferSize = Math.min(bytesAvailable, maxBufferSize);
               buffer = new byte[bufferSize];

               // read file and write it into form...
               bytesRead = fileInputStream.read(buffer, 0, bufferSize);

              while (bytesRead > 0) {

                   dos.write(buffer, 0, bufferSize);
                   dos.flush();
                   bytesAvailable = fileInputStream.available();
                   bufferSize = Math.min(bytesAvailable, maxBufferSize);
                   bytesRead = fileInputStream.read(buffer, 0, bufferSize);

               }


                // send multipart form data necesssary after file data...


                // Responses from the server (code and message)
                serverResponseCode = conn.getResponseCode();
                final String serverResponseMessage = conn.getResponseMessage();

                Log.i("uploadFile", "HTTP Response is : "
                        + serverResponseMessage + ": " + serverResponseCode);

                if(serverResponseCode == 200){

                    runOnUiThread(new Runnable() {
                        public void run() {

                            Toast.makeText(MainActivity.this, "File Upload Complete.", Toast.LENGTH_SHORT).show();
                            //Toast.makeText(MainActivity.this, serverResponseMessage, Toast.LENGTH_SHORT).show();
                        }
                    });
                }

                //close the streams //
                //fileInputStream.close();

                dos.close();

            } catch (MalformedURLException ex) {


                ex.printStackTrace();

                runOnUiThread(new Runnable() {
                    public void run() {

                        Toast.makeText(MainActivity.this, "MalformedURLException", Toast.LENGTH_SHORT).show();
                    }
                });

                Log.e("Upload file to server", "error: " + ex.getMessage(), ex);
            } catch (Exception e) {


                e.printStackTrace();

                runOnUiThread(new Runnable() {
                    public void run() {
                        Toast.makeText(MainActivity.this, "Got Exception : see logcat ", Toast.LENGTH_SHORT).show();
                    }//upload file to server exception
                });

            }

            return 0;
            //serverResponseCode;

       } // End else block
    }

}




