package com.example.quiet_home;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;


public class MainActivity extends AppCompatActivity {

    public static final String EXTRA_MESSAGE = "com.example.quiet_home.MESSAGE";
    private Button CreateNewHomeButton;

    //linking to the Firebase
    FirebaseDatabase database = FirebaseDatabase.getInstance();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        //Creating the Button Format to create a new room
        CreateNewHomeButton = (Button) findViewById(R.id.create_home_activity);
        CreateNewHomeButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Log.d("ERROR:", "Entered OnCLICK");
                launchActivity();
            }
        });

    }

    // action when the button is clocked
    private void launchActivity() {
        Intent intent = new Intent(this, devices_page.class);
        //get home name
        EditText home_name = (EditText) findViewById(R.id.HomeNameField);
        String home_name_string = home_name.getText().toString();
        home_name_string = home_name_string.replace(' ', '-');

        //get pin number
        EditText pin = (EditText) findViewById(R.id.PinField);
        String pin_string = pin.getText().toString();

        //get email
        EditText email = (EditText) findViewById(R.id.EmailField);
        String email_string = email.getText().toString();
        if(email_string.contains("@")) {
            email_string = email_string.substring(0, email_string.indexOf('@'));
        }

        //create paths
        String pin_path = "/homes/" + home_name_string + "/pin";
        String email_path = "/homes/" + home_name_string + "/email/" + email_string;
        String quiet_path = "/homes/" + home_name_string + "/quiet";


        DatabaseReference homeref = database.getReference("/homes");
        String finalHome_name_string = home_name_string;
        ValueEventListener listener = new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                // Get Post object and use the values to update the UI
                if(dataSnapshot.hasChild(finalHome_name_string)){
                    String correct_pin = dataSnapshot.child(finalHome_name_string).child("pin").getValue().toString();
                    if(correct_pin.equals(pin_string)){
                        DatabaseReference emailref = database.getReference(email_path);
                        emailref.setValue(1);

                        intent.putExtra(EXTRA_MESSAGE, finalHome_name_string);
                        startActivity(intent);
                    }
                    else{
                        Context context = getApplicationContext();
                        CharSequence text = "Incorrect Pin For " + finalHome_name_string;
                        int duration = Toast.LENGTH_SHORT;

                        Toast toast = Toast.makeText(context, text, duration);
                        toast.show();
                    }
                }
                else{
                    //create new home
                    DatabaseReference pinref = database.getReference(pin_path);
                    DatabaseReference emailref = database.getReference(email_path);
                    DatabaseReference quietref = database.getReference(quiet_path);

                    pinref.setValue(pin_string);
                    emailref.setValue(1);
                    quietref.setValue(0);

                    intent.putExtra(EXTRA_MESSAGE, finalHome_name_string);
                    startActivity(intent);
                }
            }

            @Override
            public void onCancelled(DatabaseError databaseError) {
                // Getting Post failed, log a message
                Log.w("ERROR:", "loadPost:onCancelled", databaseError.toException());
                // ...
            }
        };
        homeref.addListenerForSingleValueEvent(listener);

    }

}