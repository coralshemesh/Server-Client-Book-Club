package bgu.spl.net.srv;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public class User {

    private int connectId;
    private String userName;
    private String password;
    private boolean isConnected;
    private Map<String,String> genreSubId; //the subscription id for each topic the user subscribed

    public User(String userName, String password){
        this.userName = userName;
        this.password = password;
        connectId=-1;
        isConnected= false;
        genreSubId = new ConcurrentHashMap<>();
    }

    public void setConnectId(int connectId){ this.connectId= connectId;}

    public String getUserName(){return userName;}

    public int getConnectId(){return connectId;}

    public boolean isEqualPass(String password){ return this.password.equals(password);}

    public void setPassword(String password) {
        this.password = password;
    }

    public void setUserName(String userName) {
        this.userName = userName;
    }

    public void addToGenreSubId(String id, String topic){
        genreSubId.put(id,topic);
    }

    public String removeFromGenreSubId(String id){
        String topic = genreSubId.get(id);
        genreSubId.remove(id);
        return topic;
    }

    public void setConnected(boolean connected) {
        isConnected = connected;
    }

    public boolean isConnected(){return  isConnected;} //check if the user already logged in

    public String getIdSub(String topic){
        String ans="";
        for(String key : genreSubId.keySet()){
            if(genreSubId.get(key).equals(topic)){
                ans = key;
                break;
            }
        }
        return ans;
    }

    public void clearGenreMap(){genreSubId.clear();}


}
