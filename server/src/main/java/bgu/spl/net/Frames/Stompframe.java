package bgu.spl.net.Frames;

//import org.apache.commons.lang3.StringUtils;

import java.util.Map;
import java.util.concurrent.atomic.AtomicInteger;

public class Stompframe implements Frame {

    private String action;
    private Map<String, String> headers;
    private String body;
    private static AtomicInteger messageCounter=new AtomicInteger(0);

    public Stompframe(String action, Map headers, String body){
        this.action=action;
        this.headers=headers;
        this.body=body;
    }

    public String toString(){
        String result = action + "\n";
        for (String s : headers.keySet()){
            result = result + s + ":" + headers.get(s) + "\n";
        }
        result += "\n" + body +"\n"+ "\0";
        return result;
    }

    public String getAction() {
        return action;
    }

    public String getBody() {
        return body;
    }

    public Map<String, String> getHeaders() {
        return headers;
    }

    public String getMessageId(){return messageCounter.toString();}

    public void incrementMSGcounter(){messageCounter.incrementAndGet();}

}
