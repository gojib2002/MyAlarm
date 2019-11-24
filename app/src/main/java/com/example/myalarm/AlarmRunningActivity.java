package com.example.myalarm;

import android.app.Service;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.media.SoundPool;
import android.os.IBinder;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.View;
import android.view.inputmethod.EditorInfo;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Switch;
import android.widget.TextView;

import java.time.Instant;

import androidx.appcompat.app.AppCompatActivity;

public class AlarmRunningActivity extends AppCompatActivity {

    static MediaPlayer MP;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_alarm_running);

//        Intent intent = this.getPackageManager().getLaunchIntentForPackage("com.Test.sampleAR");
//        intent.setAction(Intent.ACTION_MAIN);
//        startActivity(intent);
//
//        finish();

        MP= MediaPlayer.create(this, R.raw.alarm1);
        MP.setLooping(true);

        MP.start();

        Button cancel = (Button) findViewById(R.id.cancel);

        cancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                MP.stop();
                finish();
            }
        });
    }
}
