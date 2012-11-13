package com.example.udptester;



import android.os.AsyncTask;
import android.os.Bundle;
import android.os.StrictMode;
import android.annotation.TargetApi;
import android.app.Activity;
import android.view.Menu;
import android.view.View;
import android.widget.TextView;

import com.org.iitd.packetx.Data;
import com.org.iitd.packetx.IPHeader;
import com.org.iitd.packetx.UDPHeader;
import com.org.iitd.packetx.UdpPacket;
import com.org.iitd.socketx.UdpSocketX;

public class MainActivity extends Activity {

	static{
		System.loadLibrary("packetcrafter");
	}
	
	private TextView sip;
	private TextView dip;
	private TextView sport;
	private TextView dport;
	private TextView data;
	private TextView reply;
	
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        sip = (TextView)this.findViewById(R.id.sip);
        sport = (TextView)this.findViewById(R.id.sport);
        dip = (TextView)this.findViewById(R.id.dip);
        dport = (TextView)this.findViewById(R.id.dport);
        data = (TextView)this.findViewById(R.id.data);
        reply = (TextView)this.findViewById(R.id.reply);
        
        //StrictMode.ThreadPolicy policy = new StrictMode.ThreadPolicy.Builder().permitAll().build();

        //StrictMode.setThreadPolicy(policy);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.activity_main, menu);
        return true;
    }
    
    public void clicked(View v){
    	String d = data.getText().toString();
    	String s = sip.getText().toString();
    	String sp = sport.getText().toString();
    	String de = dip.getText().toString();
    	String dp = dport.getText().toString();
    	
    	UdpSocketX  sx = new UdpSocketX();
    	IPHeader iph = new IPHeader(0, 65941, 0, 0, s, de);
    	UDPHeader udph = new UDPHeader(Integer.parseInt(sp), Integer.parseInt(dp));
    	Data dat = new Data(d);
    	UdpPacket upack = new UdpPacket(iph, udph, dat);
    	sx.sendPacket(upack);
    	
    	
    	//UdpPacket up1 = sx.getReplyUdpPacket();
    	//Data repl = up1.getData();
    	
    	//reply.setText(repl.getContent());
    	new UDPTask().execute(sx);
    }
    
    private class UDPTask extends AsyncTask<UdpSocketX, Void, UdpPacket> {
        protected UdpPacket doInBackground(UdpSocketX... sx) {
            return sx[0].getUdpPacket();
        }

        protected void onPostExecute(UdpPacket result) {
            Data repl = result.getData();
            reply.setText(repl.getContent());
        }
    }
}


