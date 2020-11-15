package bgu.spl.net.srv;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;


public class DataStructure {

    private Map<String, User> userMap; // user name : the user

    private static class SingletonHolder{
        private static DataStructure instance = new DataStructure();
    }

    private DataStructure(){
        userMap = new ConcurrentHashMap<>();
    }

    public static DataStructure getInstance(){
        return SingletonHolder.instance;
    }

    public boolean isExist(String name){
        if(userMap.containsKey(name)) return true;
        return false;
    }

    public void addUser(User u){
        userMap.put(u.getUserName(), u);
    }

    public boolean isConnected(String name){
        return userMap.get(name).getConnectId()!= -1;
    } //check if there is connection

    public User getUser (String name){ return userMap.get(name);}

    public User getUserById(int connectionId)
    {
        for(User user:userMap.values())
            if (user.getConnectId()==connectionId)
                return user;
            return null;
    }
}
