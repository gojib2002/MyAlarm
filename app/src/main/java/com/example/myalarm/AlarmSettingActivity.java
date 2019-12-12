package com.example.myalarm;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.nfc.Tag;
import android.os.Environment;
import android.provider.MediaStore;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.Spinner;
import android.widget.SpinnerAdapter;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.OutputStream;
import java.util.ArrayList;

import androidx.appcompat.app.AppCompatActivity;

import static android.app.PendingIntent.getActivity;


public class AlarmSettingActivity extends AppCompatActivity {

    static int hour;
    static int minute;
    static EditText tv1;
    static TextView tv2;
    static EditText tv3;
    static boolean on = false;
    static boolean isNew = false;
    private static final int REQUEST_IMAGE_1 = 1;
    static ImageButton imagebutton;
    static int AlarmNumber;
    static Spinner spinner;
    public static Bitmap image;
    private Context thisContext = this;
    int option;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_alarm_setting);

        Button downhour = (Button) findViewById(R.id.button4);
        Button upminute = (Button) findViewById(R.id.button3);
        Button downminute = (Button) findViewById(R.id.button2);
        Button uphour = (Button) findViewById(R.id.button);
        Button save = (Button) findViewById(R.id.save);
        Button cancel = (Button) findViewById(R.id.cancel);
        tv1 = (EditText) findViewById(R.id.textView);
        tv2 = (TextView) findViewById(R.id.textView3);
        tv3 = (EditText) findViewById(R.id.textView2);
        imagebutton = (ImageButton) findViewById(R.id.imageButton);
        spinner = (Spinner) findViewById(R.id.spinner);

        Intent intent = getIntent();

        hour = intent.getIntExtra("HOUR", -1);
        minute = intent.getIntExtra("MINUTE", 0);
        on = intent.getBooleanExtra("ON", false);
        AlarmNumber = intent.getIntExtra("AlarmNumber", -1);
        option = intent.getIntExtra("OPTION", 0);
        getIntent().removeExtra("RETHOUR");
        getIntent().removeExtra("RETMINUTE");
        getIntent().removeExtra("RETON");

        spinner.setSelection(option);

        image = BitmapFactory.decodeResource(getResources(), R.drawable.arimage);
        imagebutton.setImageBitmap(image);

        if(hour == -1){
            isNew = true;
            hour = 12;
        }

        setTime();

        uphour.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                hour = getHour();
                minute = getMinute();

                hour++;
                hour %= 24;
                setTime();
            }
        });

        downhour.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                hour = getHour();
                minute = getMinute();

                hour--;
                hour = (hour + 24) % 24;
                setTime();
            }
        });

        upminute.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                hour = getHour();
                minute = getMinute();

                minute++;
                if (minute == 60) {
                    hour++;
                    minute = 0;
                }
                setTime();
            }
        });

        downminute.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                hour = getHour();
                minute = getMinute();

                minute--;
                if (minute == -1) {
                    hour--;
                    minute = 59;
                }
                setTime();
            }
        });

        save.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {



                Intent intent = new Intent(getApplicationContext(), MainActivity.class);

                intent.putExtra("RETHOUR", getHour());
                intent.putExtra("RETMINUTE", getMinute());
                intent.putExtra("RETON", true);
                intent.putExtra("OPTION", option);

                if(image != null && AlarmNumber != -1) {
                    String filename = "image" + AlarmNumber + ".PNG";
                    File file = new File(filename);
                    FileOutputStream filestream = null;
                    try {
                        filestream = new FileOutputStream(file);
                        image.compress(Bitmap.CompressFormat.PNG, 0, filestream);
                    } catch (FileNotFoundException e) {
                        e.printStackTrace();
                    }
                }

                setResult(RESULT_OK, intent);

                finish();
            }
        });

        cancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                on = false;

                Intent intent = new Intent(getApplicationContext(), MainActivity.class);

                intent.putExtra("RETHOUR", getHour());
                intent.putExtra("RETMINUTE", getMinute());
                intent.putExtra("RETON", false);

                setResult(RESULT_CANCELED, intent);

                finish();
            }
        });

        imagebutton.setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View v) {
                OutputStream outStream = null;

                try{
                    outStream = new FileOutputStream(getExternalCacheDir().getAbsolutePath()+"arimage.PNG");
                    image.compress(Bitmap.CompressFormat.PNG, 100, outStream);
                    outStream.flush();
                    outStream.close();

                    Toast.makeText(thisContext,"downloaded", Toast.LENGTH_SHORT).show();
                }catch(Exception e){
                }
            }
        });

        spinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                option = position;

                if(position == 0){
                    imagebutton.setVisibility(View.VISIBLE);
                }else{
                    imagebutton.setVisibility(View.INVISIBLE);
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (resultCode == Activity.RESULT_OK) {

            switch (requestCode) {
                case REQUEST_IMAGE_1:
                    try {
                        image = MediaStore.Images.Media.getBitmap(getApplicationContext().getContentResolver(), data.getData());
                        imagebutton.setImageBitmap(image);

                        for(int i=0;i<MainActivity.AlarmList.size();i++){
                            if(MainActivity.AlarmList.get(i).AlarmNumber == AlarmNumber){
                                MainActivity.AlarmList.get(i).image = image;
                                return;
                            }
                        }
                    } catch(Exception e){

                    }
                    break;
            }
        }
    }


    public int getHour(){
        int hour = 0;
        String str = tv1.getText().toString();
        for(int i=0;i<str.length();i++){
            if(str.charAt(i) == ' ') continue;

            hour *= 10;
            hour += str.charAt(i) - '0';
        }
        hour = (hour + 24*1000) % 24;

        return hour;
    }

    public int getMinute(){
        int minute = 0;
        String str = tv3.getText().toString();
        for(int i=0;i<str.length();i++){
            if(str.charAt(i) == ' ') continue;

            minute *= 10;
            minute += str.charAt(i) - '0';
        }
        minute = (minute + 60*1000) % 60;

        return minute;
    }

    public void setTime(){
        hour = (hour + 24*1000) % 24;
        minute = (minute + 60*1000) % 60;

        if (hour < 10) tv1.setText("  0" + hour);
        else tv1.setText("  " + hour);
        if (minute < 10) tv3.setText("  0" + minute);
        else tv3.setText("  " + minute);
    }
}
