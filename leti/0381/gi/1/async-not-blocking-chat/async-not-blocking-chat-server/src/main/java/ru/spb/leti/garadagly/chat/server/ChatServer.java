package ru.spb.leti.garadagly.chat.server;

import ru.spb.leti.garadagly.chat.server.config.ServerConfiguration;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.channels.AsynchronousChannelGroup;
import java.nio.channels.AsynchronousServerSocketChannel;
import java.nio.channels.AsynchronousSocketChannel;
import java.nio.channels.CompletionHandler;
import java.util.concurrent.CopyOnWriteArraySet;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicInteger;

public class ChatServer implements Runnable {

    private final Logger logger = LoggerFactory.getLogger(this.getClass());
    private AsynchronousServerSocketChannel serverSocketListener;
    private String host;
    private int port;
    private int nThreads;
    private CopyOnWriteArraySet<ClientHandler> clients;
    private AtomicInteger broadcastCount = new AtomicInteger(0);

    public ChatServer(ServerConfiguration configuration) {
        this.nThreads = configuration.getThreads();
        this.host = configuration.getHost();
        this.port = configuration.getPort();
        clients = new CopyOnWriteArraySet<>();

        logger.info("������ ������� �� ��������� �������������: \n\t ����: {} \n\t ����: {} \n\t ���-�� �������: {}",
                host, port, nThreads);
    }

    @Override
    public void run() {
        try {
            AsynchronousChannelGroup threadGroup =
                    AsynchronousChannelGroup.withFixedThreadPool(nThreads, Executors.defaultThreadFactory());
            serverSocketListener = AsynchronousServerSocketChannel.open(threadGroup);
            serverSocketListener.bind(new InetSocketAddress(host, port));
            logger.info("������ ������������ �� {}:{}", host, port);
            serverSocketListener.accept(null, new ConnectionHandler());
        } catch (IOException e) {
            logger.error("�� ������� ��������� ������", e);
        }
    }

    public void registerClient(ClientHandler client) {
        clients.add(client);
    }

    public void unregisterClient(ClientHandler client) {
        clients.remove(client);
    }

    public synchronized void broadcast(ClientHandler who, byte[] message) {
        int currentBroadcastCount = 0;
        for (ClientHandler client : clients){
            if (!who.equals(client)) {
                client.write(message);
                currentBroadcastCount++;
            }
        }

        logger.info("�������: {}; ����������: {}; ����� ���������: {}",
                clients.size(),
                currentBroadcastCount,
                broadcastCount.incrementAndGet());
    }


    private class ConnectionHandler implements CompletionHandler<AsynchronousSocketChannel, Void> {
        @Override
        public void completed(AsynchronousSocketChannel socketChannel, Void server) {
            try {
                logger.info("������� ���������� �� {}", socketChannel.getRemoteAddress());
                serverSocketListener.accept(null, this);
                ClientHandler client = new ClientHandler(socketChannel, ChatServer.this);
                registerClient(client);
                logger.info("������ {} ���������������", socketChannel.getRemoteAddress());
                client.listen();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        @Override
        public void failed(Throwable e, Void attach) {
            logger.error("�� ������� ������� ����������");
            e.printStackTrace();
        }
    }
}
