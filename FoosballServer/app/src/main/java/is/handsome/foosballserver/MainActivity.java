package is.handsome.foosballserver;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.media.AudioManager;
import android.media.SoundPool;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v4.content.LocalBroadcastManager;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.text.format.Formatter;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.TextView;

import java.io.IOException;

import is.handsome.foosballserver.server.Server;

public class MainActivity extends AppCompatActivity {

    private TextView scoreATextView;
    private TextView scoreBTextView;
    private Button resetButton;

    private SoundPool soundPool;
    boolean soundLoaded;
    private int soundId;

    private Server server;

    private BroadcastReceiver dataReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            if (intent.getIntExtra(Server.RESET, -1) == Server.RESET_COMMAND) {
                updateScoreViews("0", "0");
            } else {
                updateScoreViews(intent.getStringExtra(Server.SCORE_A),
                        intent.getStringExtra(Server.SCORE_B));
                playSoundEffect();
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });

        scoreATextView = (TextView) findViewById(R.id.score_1_text_view);
        scoreBTextView = (TextView) findViewById(R.id.score_2_text_view);
        ((TextView) findViewById(R.id.info_text_view)).setText(getIpAddress());
        resetButton = (Button) findViewById(R.id.reset_button);
        resetButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                updateScoreViews("-", "-");
            }
        });

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        // Set the hardware buttons to control the music
        this.setVolumeControlStream(AudioManager.STREAM_MUSIC);
        // Load the sound
        AudioManager audioManager = (AudioManager) getSystemService(AUDIO_SERVICE);
        soundPool = new SoundPool(10, AudioManager.STREAM_MUSIC, 0);
        soundPool.setOnLoadCompleteListener(new SoundPool.OnLoadCompleteListener() {
            @Override
            public void onLoadComplete(SoundPool soundPool, int sampleId,
                                       int status) {
                soundLoaded = true;
            }
        });
        soundId = soundPool.load(this, R.raw.gol_sound, 1);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    @Override
    protected void onStart() {
        super.onStart();
        LocalBroadcastManager.getInstance(this)
                .registerReceiver(dataReceiver, new IntentFilter(Server.INTENT_FILTER_ACTION));
    }

    @Override
    public void onResume() {
        super.onResume();
        try {
            server = new Server(this);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void onPause() {
        super.onPause();
        if (server != null) {
            server.stop();
            server = null;
        }
    }

    @Override
    protected void onStop() {
        super.onStop();
        LocalBroadcastManager.getInstance(this).unregisterReceiver(dataReceiver);
    }

    public String getIpAddress() {
        WifiManager wifiMgr = (WifiManager) getSystemService(WIFI_SERVICE);
        int ip = wifiMgr.getConnectionInfo().getIpAddress();
        return Formatter.formatIpAddress(ip);
    }

    private void updateScoreViews(String score_1, String score_2) {
        scoreATextView.setText(score_1);
        scoreBTextView.setText(score_2);
    }

    private void playSoundEffect() {
        AudioManager audioManager = (AudioManager) getSystemService(AUDIO_SERVICE);
        float actualVolume = (float) audioManager
                .getStreamVolume(AudioManager.STREAM_MUSIC);
        float maxVolume = (float) audioManager
                .getStreamMaxVolume(AudioManager.STREAM_MUSIC);
        float volume = actualVolume / maxVolume;
        // Is the sound already?
        if (soundLoaded) {
            soundPool.play(soundId, volume, volume, 1, 0, 1f);
            Log.i("Test", "Played sound");
        }
    }
}
