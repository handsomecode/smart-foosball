package is.handsome.foosballserver;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.media.AudioManager;
import android.media.SoundPool;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.content.LocalBroadcastManager;
import android.support.v7.app.AppCompatActivity;
import android.text.format.Formatter;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.WindowManager;
import android.widget.TextView;
import android.widget.Toast;

import com.codetroopers.betterpickers.radialtimepicker.RadialTimePickerDialogFragment;
import com.hoho.android.usbserial.driver.UsbSerialDriver;
import com.hoho.android.usbserial.driver.UsbSerialPort;
import com.hoho.android.usbserial.driver.UsbSerialProber;
import com.hoho.android.usbserial.util.HexDump;
import com.hoho.android.usbserial.util.SerialInputOutputManager;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import butterknife.Bind;
import butterknife.ButterKnife;
import butterknife.OnClick;
import is.handsome.foosballserver.server.Server;

public class MainActivity extends AppCompatActivity {

    private static final int GAME_TIME_DEFAULT = 5 * 60 * 1000 + 12 * 1000;
    private static final int UPDATE_INTERVAL = 1000;
    private static final boolean NOT_RUN_AFTER_CREATION = false;

    //    @Bind(R.id.score_A_text_view) TextView scoreATextView;
//    @Bind(R.id.score_B_text_view) TextView scoreBTextView;
//    @Bind(R.id.ip_address_text_view) TextView ipAddressTextView;
    @Bind(R.id.game_timer_text_view) TextView gameTimerTextView;
    @Bind(R.id.score_a_scoreboard_double_view) ScoreboardDoubleView scoreboardADoubleView;
    @Bind(R.id.score_b_scoreboard_double_view) ScoreboardDoubleView scoreboardBDoubleView;
    @Bind(R.id.output_text_view) TextView usbOutputTextView;

    private SoundPool soundPool;
    boolean soundLoaded;
    private int soundId;

    private Server server;
    private Score score;

    private CountDownTimerWithPause countDownTimerWithPause;


    /**
     *
     */
    private static UsbSerialPort sPort;
    private final ExecutorService mExecutor = Executors.newSingleThreadExecutor();

    private SerialInputOutputManager mSerialIoManager;

    private final SerialInputOutputManager.Listener mListener =
            new SerialInputOutputManager.Listener() {

                @Override
                public void onRunError(Exception e) {
                    Log.d("TAG", "Runner stopped.");
                }

                @Override
                public void onNewData(final byte[] data) {
                    MainActivity.this.runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            MainActivity.this.updateReceivedData(data);
                        }
                    });
                }
            };

    private BroadcastReceiver dataReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            int side = intent.getIntExtra(Server.SIDE, -1);
            if (side == Server.SIDE_A) {
//                if (countDownTimerWithPause.timeLeft() != 0) {
                    score.increaseSideA();
                    scoreboardADoubleView.next();
                    updateScoreViews();
                    playSoundEffect();
//                }
            } else if (side == Server.SIDE_B) {
//                if (countDownTimerWithPause.timeLeft() != 0) {
                    score.increaseSideB();
                    scoreboardBDoubleView.next();
                    updateScoreViews();
                    playSoundEffect();
//                }
            } else if (intent.getIntExtra(Server.RESET, -1) == Server.RESET_COMMAND) {
                score.reset();
                updateScoreViews();
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.content_scoreboard);
        ButterKnife.bind(this);

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
//        ipAddressTextView.setText(getIpAddress());

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

        score = new Score();
        countDownTimerWithPause = new GameCountDownTimer(GAME_TIME_DEFAULT, UPDATE_INTERVAL, NOT_RUN_AFTER_CREATION);

//        UsbManager manager = (UsbManager) getSystemService(Context.USB_SERVICE);
//        List<UsbSerialDriver> availableDrivers = UsbSerialProber.getDefaultProber().findAllDrivers(manager);
//        if (availableDrivers.isEmpty()) {
//            Log.w("TAG", "USB devices not found");
//        }
//
//        UsbSerialDriver driver = availableDrivers.get(0);
//        UsbDeviceConnection connection = manager.openDevice(driver.getDevice());
//        if (connection == null) {
//            // You probably need to call UsbManager.requestPermission(driver.getDevice(), ..)
//            Log.w("TAG", "USB connection not establish");
//        }
//
//        // Read some data! Most have just one port (port 0).
//        UsbSerialPort port = driver.getPorts().get(0);
//        Toast.makeText(this, )

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

//        if (sPort == null) {
//            usbOutputTextView.setText("No serial device.");
//        } else {
        final UsbManager usbManager = (UsbManager) getSystemService(Context.USB_SERVICE);
        UsbSerialDriver driver = UsbSerialProber.getDefaultProber().findAllDrivers(usbManager).get(0);
        sPort = driver.getPorts().get(0);


            UsbDeviceConnection connection = usbManager.openDevice(sPort.getDriver().getDevice());
            if (connection == null) {
                usbOutputTextView.setText("Opening device failed");
                return;
            }

            try {
                sPort.open(connection);
                sPort.setParameters(115200, 8, UsbSerialPort.STOPBITS_1, UsbSerialPort.PARITY_NONE);
            } catch (IOException e) {
                Log.e("TAG", "Error setting up device: " + e.getMessage(), e);
                usbOutputTextView.setText("Error opening device: " + e.getMessage());
                try {
                    sPort.close();
                } catch (IOException e2) {
                    // Ignore.
                }
                sPort = null;
                return;
            }
            usbOutputTextView.setText("Serial device: " + sPort.getClass().getSimpleName());
//        }
        onDeviceStateChange();
    }

    @Override
    public void onPause() {
        super.onPause();
        if (server != null) {
            server.stop();
            server = null;
        }

        stopIoManager();
        if (sPort != null) {
            try {
                sPort.close();
            } catch (IOException e) {
                // Ignore.
            }
            sPort = null;
        }
    }

    @Override
    protected void onStop() {
        super.onStop();
        LocalBroadcastManager.getInstance(this).unregisterReceiver(dataReceiver);
        countDownTimerWithPause.cancel();
    }

    @SuppressWarnings("UnusedDeclaration") // used by ButterKnife
    @OnClick(R.id.game_timer_text_view)
    void onClickTimerView() {
        if (countDownTimerWithPause.timeLeft() != 0) {
            if (countDownTimerWithPause.isRunning()) {
                System.out.println("time remain = " + countDownTimerWithPause.timePassed() + " " + countDownTimerWithPause.timeLeft());
                countDownTimerWithPause.pause();
            } else {
                System.out.println("time remain = " + countDownTimerWithPause.timePassed() + " " + countDownTimerWithPause.timeLeft());
                countDownTimerWithPause.resume();
            }
        } else {
            startNewGame();
        }
    }

    @SuppressWarnings("UnusedDeclaration") // used by ButterKnife
    @OnClick(R.id.logo_image_view)
    void onClickStartGame() {
        if (countDownTimerWithPause.timeLeft() == 0) {
            showTimerSetting();
        }
//        score.reset();
//        updateScoreViews();
//        countDownTimerWithPause.cancel();
//        countDownTimerWithPause.create();
//        countDownTimerWithPause.resume();

    }


    @SuppressWarnings("UnusedDeclaration") // used by ButterKnife
    @Nullable
    @OnClick(R.id.set_timer_button)
    void onClickSetTimer() {
        RadialTimePickerDialogFragment pickerFragment = RadialTimePickerDialogFragment
                .newInstance(new RadialTimePickerDialogFragment.OnTimeSetListener() {
                    @Override
                    public void onTimeSet(RadialTimePickerDialogFragment dialog, int hourOfDay, int minute) {
                        long time = hourOfDay * 60 * 1000 + minute * 1000;
                        countDownTimerWithPause = new GameCountDownTimer(time, UPDATE_INTERVAL, NOT_RUN_AFTER_CREATION);
                    }
                }, 5, 12, true);
        pickerFragment.setThemeCustom(R.style.MyCustomBetterPickersRadialTimePickerDialog);
        pickerFragment.show(getSupportFragmentManager(), "picker");
    }

    private String getIpAddress() {
        WifiManager wifiMgr = (WifiManager) getSystemService(WIFI_SERVICE);
        int ip = wifiMgr.getConnectionInfo().getIpAddress();
        return Formatter.formatIpAddress(ip);
    }

    private void updateTimerView(long millisUntilFinished) {
        System.out.println(String.valueOf(millisUntilFinished) + " " + millisUntilFinished / (60 * UPDATE_INTERVAL) + " " + (millisUntilFinished / UPDATE_INTERVAL) % 60);
        long minutes = millisUntilFinished / (60 * UPDATE_INTERVAL);
        long seconds = (millisUntilFinished / UPDATE_INTERVAL) % 60;
        gameTimerTextView.setText(String.format("%02d:%02d", minutes, seconds));
    }

    private void updateScoreViews() {
//        scoreATextView.setText(String.valueOf(score.getSideA()));
//        scoreBTextView.setText(String.valueOf(score.getSideB()));
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

    private void startNewGame() {
        scoreboardADoubleView.reset();
        scoreboardBDoubleView.reset();
        countDownTimerWithPause.cancel();
        countDownTimerWithPause.create();
        countDownTimerWithPause.resume();
    }

    private void showTimerSetting() {
        RadialTimePickerDialogFragment pickerFragment = RadialTimePickerDialogFragment
                .newInstance(new RadialTimePickerDialogFragment.OnTimeSetListener() {
                    @Override
                    public void onTimeSet(RadialTimePickerDialogFragment dialog, int hourOfDay, int minute) {
                        long time = hourOfDay * 60 * 1000 + minute * 1000;
                        countDownTimerWithPause.cancel();
                        countDownTimerWithPause = new GameCountDownTimer(time, UPDATE_INTERVAL, NOT_RUN_AFTER_CREATION);
                    }
                }, 5, 12, true);
        pickerFragment.setThemeCustom(R.style.MyCustomBetterPickersRadialTimePickerDialog);
        pickerFragment.show(getSupportFragmentManager(), "picker");
    }


    private class GameCountDownTimer extends CountDownTimerWithPause {
        /**
         * @param millisOnTimer
         * @param countDownInterval The interval in millis at which to execute
         *                          onTick(millisUntilFinished)} callbacks
         * @param runAtStart        True if timer should start running, false if not
         */
        public GameCountDownTimer(long millisOnTimer, long countDownInterval, boolean runAtStart) {
            super(millisOnTimer, countDownInterval, runAtStart);
        }

        @Override
        public void onTick(long millisUntilFinished) {
            updateTimerView(millisUntilFinished);
        }

        @Override
        public void onFinish() {
            gameTimerTextView.setText("СТАРТ");
        }
    }


    private void updateReceivedData(byte[] data) {
        String str = "";
        try {
            str = new String(data, "UTF-8");
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
            usbOutputTextView.setText("FAIL");

        }
        int b = data[0] == 48 ? 0 : data[0] == 49 ? 1 : -1;

        final String message = "Read " + data.length + " bytes: \n" + str + " " + b;
//                + HexDump.dumpHexString(data) + "\n\n";
        usbOutputTextView.setText(message);

        if(b == 0) {
            scoreboardADoubleView.next();
            playSoundEffect();
        }
        if(b == 1) {
            scoreboardBDoubleView.next();
            playSoundEffect();
        }
    }

    private void stopIoManager() {
        if (mSerialIoManager != null) {
            Log.i("TAG", "Stopping io manager ..");
            mSerialIoManager.stop();
            mSerialIoManager = null;
        }
    }

    private void startIoManager() {
        if (sPort != null) {
            Log.i("TAG", "Starting io manager ..");
            mSerialIoManager = new SerialInputOutputManager(sPort, mListener);
            mExecutor.submit(mSerialIoManager);
        }
    }

    private void onDeviceStateChange() {
        stopIoManager();
        startIoManager();
    }

}
