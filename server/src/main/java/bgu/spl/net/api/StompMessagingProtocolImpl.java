package bgu.spl.net.api;

import bgu.spl.net.Frames.Stompframe;
import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.DataStructure;
import bgu.spl.net.srv.User;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;


public class StompMessagingProtocolImpl<T> implements StompMessagingProtocol<T> {

    private boolean shouldTerminate;
    private int connectionId;
    private Connections<T> connections;
    private DataStructure dataStructure = DataStructure.getInstance();
    private User user;

    public StompMessagingProtocolImpl() {
        this.shouldTerminate = false;
        user = new User("", "");

    }

    public void start(int connectionId, Connections<T> connections) {
        this.connectionId = connectionId;
        this.connections = connections;
    }


    public void process(T message) throws IOException {
        if (!shouldTerminate) {
            Stompframe stompframe = (Stompframe) message;
            if (stompframe.getAction().equals("CONNECT") | stompframe.getAction().equals("STOMP")){
                System.out.println("Getting CONNECT frame");
                Stompframe loginResult = login(stompframe);
                connections.send(connectionId, (T)loginResult);
                if (loginResult.getAction().equals("ERROR"))  connections.removeFromIdconnect(connectionId); // sends the error and afterwards remove the user and it's connection handler
            }
            else if (user.isConnected()) {
                switch (stompframe.getAction()) {
                    case "SUBSCRIBE":
                        System.out.println("Getting SUBSCRIBE frame");
                        connections.send(connectionId, (T) subscribe(stompframe));
                        break;
                    case "UNSUBSCRIBE":
                        System.out.println("Getting UNSUBSCRIBE frame");
                        connections.send(connectionId, (T) unsubscribe(stompframe));
                        break;
                    case "SEND":
                        System.out.println("Getting SEND frame");
                        //send the message response as a broadcast to all clients subscribed to this channel
                        connections.send(stompframe.getHeaders().get("destination"), (T) sendFrame(stompframe));
                        break;
                    case "DISCONNECT":
                        System.out.println("Getting DISCONNECT frame");
                        user.setConnected(false);
                        connections.send(connectionId, (T) disconnect(stompframe));
                        connections.removeFromIdconnect(connectionId); // after sending the frame remove the connectionId and the connection handler from the map
                        break;
                }
            }

            else {
                Map<String, String> headers2 = new HashMap<>();
                headers2.put("message", "user is not logged in");
                connections.send(connectionId, (T) new Stompframe("ERROR", headers2, ""));
            }
        }
    }


    public boolean shouldTerminate() {
        return shouldTerminate;
    }

    //additional functions

    public Stompframe login(Stompframe message) {
        Map<String, String> version = new ConcurrentHashMap<>();
        version.put("version", message.getHeaders().get("accept-version"));
        String name = message.getHeaders().get("login");
        String password = message.getHeaders().get("passcode");
        if (!dataStructure.isExist(name)) { //server doesn't find the username
            this.user.setUserName(name);
            this.user.setPassword(password);
            user.setConnectId(connectionId);
            user.setConnected(true); //the user is now connected
            dataStructure.addUser(user);
            return new Stompframe("CONNECTED", version, "");
        } else {
            User oldUser = dataStructure.getUser(name);
            Map<String, String> headers = new ConcurrentHashMap<>();
            //headers.put("receipt-id", message.getHeaders().get("receipt"));
            if (oldUser.isConnected()) { //user logged in
                headers.put("message", "user already logged in");
                connections.disconnect(connectionId);
                //shouldTerminate = true;
                return new Stompframe("ERROR", headers, "");
            } else if (!oldUser.isEqualPass(password)) {
                headers.put("message", "wrong password");
                connections.disconnect(connectionId);
                //shouldTerminate=true;
                return new Stompframe("ERROR", headers, "");
            } else{
                oldUser.setConnectId(connectionId);
                oldUser.setConnected(true);
                this.user = oldUser;
                return new Stompframe("CONNECTED", version, "");
            }
        }
    }

    public Stompframe subscribe(Stompframe message) {
        String topic = message.getHeaders().get("destination");
        String receipt = message.getHeaders().get("receipt");
        String id = message.getHeaders().get("id");

        user = dataStructure.getUserById(connectionId);
        connections.addToTopic(topic, user);
        user.addToGenreSubId(id, topic);

        Map<String, String> headers = new ConcurrentHashMap<>();
        headers.put("receipt-id", receipt);

        return new Stompframe("RECEIPT", headers, "joined club " + topic);
    }


    public Stompframe unsubscribe(Stompframe message) {
        String idSub = message.getHeaders().get("id");
        String topic = user.removeFromGenreSubId(idSub);
        connections.removeFromTopic(topic, user.getConnectId());

        Map<String, String> headers = new ConcurrentHashMap<>();

        headers.put("receipt-id", message.getHeaders().get("receipt"));

        return new Stompframe("RECEIPT", headers, "exit club " + topic);
    }

    public Stompframe sendFrame(Stompframe message) {
        String topic = message.getHeaders().get("destination");
            Map<String, String> headers = new ConcurrentHashMap<>();
            headers.put("subscription","");
            headers.put("Message-id", message.getMessageId());
            headers.put("destination", topic);
            return new Stompframe("MESSAGE", headers, message.getBody());
    }

    public Stompframe disconnect(Stompframe message) {
        connections.disconnect(connectionId);
        Map<String, String> headers = new ConcurrentHashMap<>();
        headers.put("receipt-id", message.getHeaders().get("receipt"));
        return new Stompframe("RECEIPT", headers, "logout");
    }


}
