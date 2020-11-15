package bgu.spl.net.api;

import bgu.spl.net.Frames.Stompframe;
import java.nio.charset.StandardCharsets;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Arrays;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public class MessageEncoderDecoderImpl implements MessageEncoderDecoder<Stompframe> {

    private byte[] bytes = new byte[1 << 10];
    private int len = 0;


    @Override
    public Stompframe decodeNextByte(byte nextByte) {
        String fromPopString="";
        if(nextByte =='\0') {
            fromPopString = popString();
            String[] lines = fromPopString.split("\n");
            String action = lines[0];
            Map<String, String> headers = new ConcurrentHashMap<>();
            int count = 1;
            boolean lineIsEmpty = false;
            for (int i = 1; i < lines.length & !lineIsEmpty; i++) {
                if (lines[i].isEmpty()) lineIsEmpty = true;
                else {
                    String[] oneHeader = lines[i].split(":");
                    headers.put(oneHeader[0], oneHeader[1]);
                    count++;
                }
            }
            String body = "";
            while (count < lines.length && lines[count].isEmpty()) count++;

            while (count < lines.length && !lines[count].isEmpty()) {
                    body += lines[count];
                    count++;
            }

            return new Stompframe(action, headers, body);
        }
        pushByte(nextByte);
        return null;
    }


    @Override
    public byte[] encode(Stompframe message) {
        String action = message.getAction()+"\n";
        String outputHeaders="";
        Map<String,String> headers = message.getHeaders();
        for (String s : headers.keySet()){
            outputHeaders+=s +":"+headers.get(s)+"\n";
        }
        String body = message.getBody()+"\n";
        String output = action + outputHeaders + "\n" + body + "\0";

        return output.getBytes();
    }

    private String popString() {
        String result = new String(bytes, 0, len, StandardCharsets.UTF_8);
        len = 0;
        return result;
    }

    private void pushByte(byte nextByte) {
        if (len >= bytes.length) {
            bytes = Arrays.copyOf(bytes, len * 2);
        }

        bytes[len++] = nextByte;
    }

}
