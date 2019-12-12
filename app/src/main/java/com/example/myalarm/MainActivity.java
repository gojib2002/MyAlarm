package com.example.myalarm;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.os.Bundle;
import android.view.ContextMenu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.Switch;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.time.Instant;
import java.util.ArrayList;

import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {

    public static ArrayList<Alarm> AlarmList;
    static LinearLayout baseLayout;
    int CurrentButton;
    MainActivity th = this;

    public class TimeChecker implements Runnable{       //현재 시각과 설정된 시각을 비교해서 같아지면 알람을 실행해주는 Thread

        @Override
        public void run() {
            int ThreadNumber = CurrentButton;
            int AlarmNumber = AlarmList.get(CurrentButton).AlarmNumber;

            while(true) {
                try {
                    Thread.sleep(1000);
                    if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.O) {
                        if(ThreadNumber >= AlarmList.size() || AlarmList.get(ThreadNumber).AlarmNumber != AlarmNumber){
                            for(int i=0;i<AlarmList.size();i++){
                                if(AlarmList.get(i).AlarmNumber == AlarmNumber){
                                    ThreadNumber = i;
                                    break;
                                }
                            }
                        }

                        if(ThreadNumber >= AlarmList.size() || AlarmList.get(ThreadNumber).AlarmNumber != AlarmNumber) {
                            return;
                        }

                        Instant CurrentTime = Instant.now();

                        int CurrentMinute = (int) (CurrentTime.getEpochSecond() % 3600) / 60;
                        int CurrentHour = (int) ((CurrentTime.getEpochSecond() % 86400) / 3600 + 9) % 24;

                        if (CurrentHour == AlarmList.get(ThreadNumber).hour && CurrentMinute == AlarmList.get(ThreadNumber).minute && AlarmList.get(ThreadNumber).on) {

                            if(AlarmList.get(ThreadNumber).option == 0) {
                                Intent intent = th.getPackageManager().getLaunchIntentForPackage("com.DefaultCompany.UaaLExample");
                                intent.setAction(Intent.ACTION_MAIN);
                                startActivity(intent);
                            }else{
                                startActivity(new Intent(MainActivity.this, CameraActivity.class));
                            }

                            AlarmList.get(ThreadNumber).on = false;
                            break;
                        }

                        if(!AlarmList.get(ThreadNumber).on){
                            break;
                        }
                    }

                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }


            AlarmList.get(ThreadNumber).btn.setBackgroundColor(Color.WHITE);
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT,
                LinearLayout.LayoutParams.MATCH_PARENT);
        //리니어 레이아웃 정의 params

        ScrollView scrollview = new ScrollView(this);

        setContentView(scrollview);

        baseLayout = new LinearLayout(this);
        baseLayout.setOrientation(LinearLayout.VERTICAL);
        baseLayout.setBackgroundColor(Color.rgb(255,255,255));

        scrollview.addView(baseLayout);
        //레이아웃의 특성 정의
        //setContentView(baseLayout,params);
        //정의한 레이아웃의 특성을 params에 출력
//        try{
//            BufferedWriter bw = new BufferedWriter(new FileWriter(getFilesDir() + "Alarm.txt"));
//            bw.write("");
//            bw.close();
//        } catch (IOException e) {
//            e.printStackTrace();
//        }

//        AlarmList = new ArrayList<>();
        if(AlarmList == null){
            AlarmList = new ArrayList<Alarm>();

            try {
                BufferedReader br = new BufferedReader(new FileReader(getFilesDir() + "Alarm.txt"));
                while(true) {
                    String str = br.readLine();

                    if (str == null) break;

                    String[] AlarmStr = str.split(" ");     //hour minute on 순서.

                    int hour = Integer.parseInt(AlarmStr[0]);
                    int minute = Integer.parseInt(AlarmStr[1]);
                    boolean on;
                    if (AlarmStr[2].equals("true")) on = true;
                    else on = false;
                    int AlarmNumber = Integer.parseInt(AlarmStr[3]);
                    int option = Integer.parseInt(AlarmStr[4]);

                    CurrentButton = AlarmList.size();
                    AlarmList.add(new Alarm(null, hour, minute, on, AlarmNumber, option));

                    try{
                        String imgpath = "data/data/com.example.alarm2/files/" + "alarm" + AlarmNumber + ".png";
                        AlarmList.get(CurrentButton).image = BitmapFactory.decodeFile(imgpath);
                        Toast.makeText(getApplicationContext(), "load ok", Toast.LENGTH_SHORT).show();
                    }catch(Exception e){Toast.makeText(getApplicationContext(), "load error", Toast.LENGTH_SHORT).show();}
                }
                br.close();
            } catch (FileNotFoundException e) {
            } catch (IOException e) {
            }
        }

        Button btn = new Button(this);
        btn.setText("알람 추가");
        btn.setBackgroundColor(Color.WHITE);
        baseLayout.addView(btn);

        for(int i=0;i<AlarmList.size();i++){
            AlarmList.get(i).btn = new Button(this);
            AlarmList.get(i).btn.setText(AlarmList.get(i).hour + " : " + AlarmList.get(i).minute);
            if(!AlarmList.get(i).on) AlarmList.get(i).btn.setBackgroundColor(Color.WHITE);
            else AlarmList.get(i).btn.setBackgroundColor(Color.BLUE);
            baseLayout.addView(AlarmList.get(i).btn);
            AlarmList.get(i).btn.setId(AlarmList.get(i).AlarmNumber);

            AlarmList.get(i).btn.setOnClickListener(ButtonClick);
            registerForContextMenu(AlarmList.get(i).btn);

            if(AlarmList.get(i).on){
                AlarmList.get(i).on = false;
                AlarmList.get(i).AlarmStart();
            }
        }

        btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CurrentButton = AlarmList.size();
                Intent intent = new Intent(MainActivity.this, AlarmSettingActivity.class);

                int tempNumber = 1;
                for(int i=0;i<AlarmList.size();i++){
                    if(AlarmList.get(i).AlarmNumber == tempNumber){
                        tempNumber++;
                        i = -1;
                        continue;
                    }
                }

                intent.putExtra("AlarmNumber", tempNumber);

                startActivityForResult(intent, 1);
            }
        });
    }

    @Override

    public void onCreateContextMenu(ContextMenu menu, View v, ContextMenu.ContextMenuInfo menuInfo) {

        // TODO Auto-generated method stub

        getMenuInflater().inflate(R.menu.button_menu, menu);

        for(int i=0;i<AlarmList.size();i++){
            if(AlarmList.get(i).AlarmNumber == v.getId())
            {
                CurrentButton = i;
                break;
            }
        }

        super.onCreateContextMenu(menu, v, menuInfo);

    }

    @Override

    public boolean onContextItemSelected(MenuItem item) {

        // TODO Auto-generated method stub

        switch( item.getItemId() ){//눌러진 MenuItem의 Item Id를 얻어와 식별
            case R.id.delete:
                baseLayout.removeView(AlarmList.get(CurrentButton).btn);
                AlarmList.get(CurrentButton).on = false;
                AlarmList.remove(AlarmList.get(CurrentButton));
                break;

            case R.id.stop:
                AlarmList.get(CurrentButton).AlarmStop();
                break;

        }

        return super.onContextItemSelected(item);
    }

    Button.OnClickListener ButtonClick = new View.OnClickListener(){
        public void onClick(View v) {       //버튼을 클릭하면 해당하는 알람의 설정 페이지로 넘어간다.
            int i = 0;

            for(i=0;i<AlarmList.size();i++){
                if(v.getId() == AlarmList.get(i).AlarmNumber){
                    CurrentButton = i;
                    break;
                }
            }

            Toast.makeText(MainActivity.this,""+CurrentButton,Toast.LENGTH_SHORT).show();

            Intent intent = new Intent(MainActivity.this, AlarmSettingActivity.class);

            intent.putExtra("HOUR", AlarmList.get(i).hour);
            intent.putExtra("MINUTE", AlarmList.get(i).minute);
            intent.putExtra("ON", AlarmList.get(i).on);
            intent.putExtra("AlarmNumber", AlarmList.get(i).AlarmNumber);
            intent.putExtra("OPTION", AlarmList.get(i).option);

            startActivityForResult(intent, 2);
        }

    };
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        // Check which request we're responding to
        if (requestCode == 1) {
            if (resultCode == RESULT_OK) {

                int hour = data.getIntExtra("RETHOUR", -1);
                int minute = data.getIntExtra("RETMINUTE", -1);
                boolean on = data.getBooleanExtra("RETON", false);
                int option = data.getIntExtra("OPTION", 0);

                Alarm alarm = new Alarm(new Button(this), hour, minute, on, option);

                alarm.btn.setText(hour + " : " + minute);
                alarm.btn.setBackgroundColor(Color.BLUE);
                baseLayout.addView(alarm.btn);
                alarm.btn.setId(alarm.AlarmNumber);

                alarm.btn.setOnClickListener(ButtonClick);
                registerForContextMenu(alarm.btn);

                AlarmList.add(alarm);

                AlarmList.get(CurrentButton).image = AlarmSettingActivity.image;

                AlarmList.get(CurrentButton).on = false;
                AlarmList.get(CurrentButton).AlarmStart();
            }
        } else if(requestCode == 2){
            if(resultCode == RESULT_OK){
                int hour = data.getIntExtra("RETHOUR", -1);
                int minute = data.getIntExtra("RETMINUTE", -1);
                boolean on = data.getBooleanExtra("RETON", false);
                int option = data.getIntExtra("OPTION", 0);

                AlarmList.get(CurrentButton).hour = hour;
                AlarmList.get(CurrentButton).minute = minute;
                AlarmList.get(CurrentButton).btn.setText(hour + " : " + minute);
                AlarmList.get(CurrentButton).option = option;
                AlarmList.get(CurrentButton).AlarmStart();
            } else if(resultCode == RESULT_CANCELED){

                AlarmList.get(CurrentButton).AlarmStop();
                //baseLayout.removeView(AlarmList.get(CurrentButton).btn);
                //AlarmList.remove(AlarmList.get(CurrentButton));
            }
        }
    }

    @Override
    protected void onPause() {      //어플을 종료할 때 알람을 저장한다.
        super.onPause();

        try {
            BufferedWriter bw = new BufferedWriter(new FileWriter(getFilesDir() + "Alarm.txt"));

            for(int i=0;i<AlarmList.size();i++) {
                String str = AlarmList.get(i).hour + " " + AlarmList.get(i).minute + " ";
                if(AlarmList.get(i).on) str += "true";
                else str += "false";
                str += " " + AlarmList.get(i).AlarmNumber;
                str += " " + AlarmList.get(i).option;

                if(AlarmList.get(i).image != null){
                    try{

                        FileOutputStream fos = openFileOutput("alarm" + AlarmList.get(i).AlarmNumber + ".png", 0);
                        AlarmList.get(i).image.compress(Bitmap.CompressFormat.PNG, 100 , fos);
                        fos.flush();
                        fos.close();

                        Toast.makeText(this, "file ok", Toast.LENGTH_SHORT).show();
                    }catch(Exception e) { Toast.makeText(this, "file error", Toast.LENGTH_SHORT).show();}

                }

                str += "\n";

                bw.write(str);
            }

            bw.close();
        } catch (IOException e) {
        }
    }

    @Override
    protected void onDestroy() {        //어플을 종료할 때 알람을 저장한다.
        super.onDestroy();

        try {
            BufferedWriter bw = new BufferedWriter(new FileWriter(getFilesDir() + "Alarm.txt"));

            for(int i=0;i<AlarmList.size();i++) {
                String str = AlarmList.get(i).hour + " " + AlarmList.get(i).minute + " ";
                if(AlarmList.get(i).on) str += "true";
                else str += "false";
                str += " " + AlarmList.get(i).AlarmNumber;
                str += " " + AlarmList.get(i).option;

                if(AlarmList.get(i).image != null){
                    try{

                        FileOutputStream fos = openFileOutput("alarm" + AlarmList.get(i).AlarmNumber + ".png", 0);
                        AlarmList.get(i).image.compress(Bitmap.CompressFormat.PNG, 100 , fos);
                        fos.flush();
                        fos.close();

                        Toast.makeText(this, "file ok", Toast.LENGTH_SHORT).show();
                    }catch(Exception e) { Toast.makeText(this, "file error", Toast.LENGTH_SHORT).show();}

                }

                str += "\n";

                bw.write(str);
            }

            bw.close();
        } catch (IOException e) {
        }
    }

    class Alarm {
        int AlarmNumber;
        Switch sw ;
        Button btn;
        int hour;
        int minute;
        boolean on;
        int option;
        Bitmap image;
        TimeChecker tc;
        Thread t;

        Alarm(){}
        Alarm(Button btn, int hour, int minute, boolean on, int option){
            this.btn = btn;
            this.hour = hour;
            this.minute = minute;
            this.on = on;
            this.option = option;

            this.AlarmNumber = 1;
            for(int i=0;i<AlarmList.size();i++){
                if(AlarmList.get(i).AlarmNumber == this.AlarmNumber){
                    this.AlarmNumber++;
                    i = -1;
                    continue;
                }
            }
        }
        Alarm(Button btn, int hour, int minute, boolean on, int AlarmNumber, int option){
            this.btn = btn;
            this.hour = hour;
            this.minute = minute;
            this.on = on;
            this.AlarmNumber = AlarmNumber;
            this.option = option;
        }

        void AlarmStart(){
            if(this.on) return;

            this.on = true;
            tc = new TimeChecker();
            t = new Thread(tc);
            t.start();
            btn.setBackgroundColor(Color.BLUE);
        }

        void AlarmStop(){
            if(!this.on) return;

            this.on = false;
            btn.setBackgroundColor(Color.WHITE);
        }
    }
}
