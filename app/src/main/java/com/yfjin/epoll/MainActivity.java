package com.yfjin.epoll;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;

import androidx.appcompat.app.AppCompatActivity;

import com.yfjin.epoll.databinding.ActivityMainBinding;


public class MainActivity extends AppCompatActivity {

    static {
        System.loadLibrary("epollJni");
    }

    private ActivityMainBinding binding;

    public static void jump(Context c) {
        c.startActivity(new Intent(c, MainActivity.class));
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        //设备IP+设备端口
//        startEpollServer("192.168.41.140",10086);
        startSelectServer("192.168.41.140",10086);
    }

    private native void startEpollServer(String address,int port);
    private native void startSelectServer(String address,int port);

    public void onResultShow(String result) {
        Log.i("123123java", "result:" + result);
        runOnUiThread(() -> {
            binding.fab.setText("result:" + result);
        });
    }

}