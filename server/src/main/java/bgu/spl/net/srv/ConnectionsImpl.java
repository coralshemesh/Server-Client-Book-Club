package bgu.spl.net.srv;


import bgu.spl.net.Frames.Stompframe;

import java.io.IOException;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;

public class ConnectionsImpl implements Connections<Stompframe> {

    private Map<Integer, ConnectionHandler> idConnect; // id connection : connection handler
    private Map<String, ConcurrentLinkedQueue<Integer>> topicClients; //topic : a list of user names subscribed to this topic
    private DataStructure dataStructure;

    public ConnectionsImpl() {
        idConnect = new ConcurrentHashMap<>();
        topicClients = new ConcurrentHashMap<>();
        dataStructure = DataStructure.getInstance();
    }

    @Override
    public boolean send(int connectionId, Stompframe msg) throws IOException {
        ConnectionHandler handler = idConnect.get(connectionId);
        if (handler != null) {
            synchronized (handler) {
                if (idConnect.containsKey(connectionId)) {
                    idConnect.get(connectionId).send(msg);
                    return true;
                }
            }
        }
        return false;
    }

    @Override
    public void send(String channel, Stompframe msg) throws IOException {
        if (topicClients.containsKey(channel) && topicClients.get(channel) != null) {
            for (Integer connectionId : topicClients.get(channel)) {
                // update the IdSubscription of each client so we can send a correct MESSAGE frame
                String id = dataStructure.getUserById(connectionId).getIdSub(channel);
                msg.getHeaders().replace("subscription", id);
                idConnect.get(connectionId).send(msg);
                msg.incrementMSGcounter(); // messageCounter++ for every msg sent
            }
        }
    }

    @Override
    public void disconnect(int connectionId) {
        for (String s : topicClients.keySet()) {
            //remove the user from the topics he subscribed to
            if (topicClients.get(s) != null) {
                topicClients.get(s).remove((Object) connectionId);
            }
            //clear the user's genreSubId map
            if(dataStructure.getUserById(connectionId) != null) {
                dataStructure.getUserById(connectionId).clearGenreMap();
            }
        }
    }

    public void connect(ConnectionHandler connectionHandler, int id) { // add to id connect map
        idConnect.put(id, connectionHandler);
    }

    public void addToTopic(String topic, User user) {
        if (topicClients.containsKey(topic)) {
            topicClients.get(topic).add(user.getConnectId());
        } else {
            ConcurrentLinkedQueue<Integer> idConnections = new ConcurrentLinkedQueue<>();
            idConnections.add(user.getConnectId());
            topicClients.put(topic, idConnections);
        }
    }

    public void removeFromTopic(String topic, Integer id) {
        if (topicClients.containsKey(topic) && topicClients.get(topic).contains(id))
            topicClients.get(topic).remove(id);
    }

    public void removeFromIdconnect(int connectionId) {
        ConnectionHandler handler = idConnect.get(connectionId);
        if (handler != null) {
            synchronized (handler) {
                idConnect.remove(connectionId);
            }
        }
    }


}
