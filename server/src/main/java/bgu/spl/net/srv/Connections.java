package bgu.spl.net.srv;

import bgu.spl.net.Frames.Stompframe;

import java.io.IOException;

public interface Connections<T> {

    boolean send(int connectionId,T  msg) throws IOException;

    void send(String channel, T msg) throws IOException;

    void disconnect(int connectionId);

    void connect(ConnectionHandler c, int i);

    void addToTopic(String topic, User user);

    void removeFromTopic(String topic, Integer id);

    void removeFromIdconnect(int connectionId);


}
