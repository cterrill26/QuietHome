package com.example.quiet_home;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.NotificationCompat;

import android.app.ActionBar;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

import java.util.ArrayList;
import java.util.List;

public class devices_page extends AppCompatActivity {

    private int device_counter = 0;
    FirebaseDatabase database = FirebaseDatabase.getInstance();
    private Button addDeviceButton;
    String HomeName;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_devices_page);

        //Get Name of the Home Device///////////////////////////////////
        Intent intent = getIntent();
        HomeName = intent.getStringExtra(MainActivity.EXTRA_MESSAGE);
        TextView textView = findViewById(R.id.home_name);
        textView.setText(HomeName);
        ////////////////////////////////////////////////////////////

        ////Load in all the active devices for this home Device////////
        loadin();

        //////////////////////////////////////////////////////////////

        //Behavior when we click the add device button//////////////////
        addDeviceButton = (Button) findViewById(R.id.add_device_button);
        addDeviceButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
//                Log.d("ERROR:", "Entered OnCLICK");
                addDevice();
            }
        });
        /////////////////////////////////////////////////////////////////
    }



    private void loadin() {
        String device_path = "/homes/" + HomeName + "/devices";
        DatabaseReference device_ref = database.getReference(device_path);

        ValueEventListener listener = new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                LinearLayout linearLayout = findViewById(R.id.linear_layout);
                linearLayout.removeViews(1, linearLayout.getChildCount() - 1);
                Iterable<DataSnapshot> devices = dataSnapshot.getChildren();
                for(DataSnapshot device : devices){
                    LinearLayout ll = new LinearLayout(getApplicationContext());
                    ll.setLayoutParams(new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT));
                    ll.setPadding(15, 15, 15, 15);
                    ll.setOrientation(LinearLayout.HORIZONTAL);
                    TextView ser =new TextView(getApplicationContext());
                    TextView name =new TextView(getApplicationContext());
                    ser.setText(device.getKey());
                    name.setText(device.getValue().toString());
                    ser.setTextColor(getResources().getColor(R.color.teal_700));
                    name.setTextColor(getResources().getColor(R.color.teal_700));
                    ser.setTextSize(TypedValue.COMPLEX_UNIT_DIP, 20);
                    name.setTextSize(TypedValue.COMPLEX_UNIT_DIP, 20);
                    ser.setGravity(Gravity.CENTER);
                    name.setGravity(Gravity.CENTER);
                    ser.setLayoutParams(new LinearLayout.LayoutParams(0, LinearLayout.LayoutParams.WRAP_CONTENT, 1));
                    name.setLayoutParams(new LinearLayout.LayoutParams(0, LinearLayout.LayoutParams.WRAP_CONTENT, 1));
                    ll.addView(ser);
                    ll.addView(name);
                    linearLayout.addView(ll);
                }
            }

            @Override
            public void onCancelled(DatabaseError databaseError) {
                // Getting Post failed, log a message
                Log.w("ERROR:", "loadPost:onCancelled", databaseError.toException());
                // ...
            }
        };
        device_ref.addListenerForSingleValueEvent(listener);
    }


    private void addDevice() {

        //grab serial number from user
        EditText device_name = (EditText) findViewById(R.id.add_device_name);
        EditText device_serial = (EditText) findViewById(R.id.add_device_serial);
        String serial_number_string = device_serial.getText().toString();
        String device_name_string = device_name.getText().toString();
        device_name.setText("");
        device_serial.setText("");

        //check if serial number exists in the database
        DatabaseReference myRef = database.getReference("/devices");

        ValueEventListener listener = new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                // Get Post object and use the values to update the UI
                if(dataSnapshot.hasChild(serial_number_string)){
                    String status = dataSnapshot.child(serial_number_string).getValue().toString();
                    if(status.equals("")){
                        DatabaseReference device_status = database.getReference("/devices/" + serial_number_string);
                        device_status.setValue(HomeName);

                        DatabaseReference home_device_entry = database.getReference("/homes/" + HomeName + "/devices/" + serial_number_string);
                        home_device_entry.setValue(device_name_string);
                        loadin();
                    }
                    else{
                        Context context = getApplicationContext();
                        CharSequence text = "Device In Use";
                        int duration = Toast.LENGTH_SHORT;

                        Toast toast = Toast.makeText(context, text, duration);
                        toast.show();
                    }
                }
                else{
                    Context context = getApplicationContext();
                    CharSequence text = "Invalid Serial Number";
                    int duration = Toast.LENGTH_SHORT;

                    Toast toast = Toast.makeText(context, text, duration);
                    toast.show();
                }
            }

            @Override
            public void onCancelled(DatabaseError databaseError) {
                // Getting Post failed, log a message
                Log.w("ERROR:", "loadPost:onCancelled", databaseError.toException());
                // ...
            }
        };
        myRef.addListenerForSingleValueEvent(listener);

    }
}