
public class Channel<E> {
	E message;

	Boolean message_sent;
	Boolean message_received;

	public synchronized void send(E message) {
		message_sent = Boolean.FALSE;
		this.message = message;

		synchronized (message_sent) { notify(); }
		synchronized (message_received) {
			if (! message_received.booleanValue()) {
				message_sent = Boolean.TRUE;
				wait(); /* On a receiver */
			}
		}
	} 

	public synchronized E receive() {
		/* If no message was sent wait for a message to come in */
		if (! message_sent.booleanValue()) {
			synchronized (message_sent) {
				wait(); /* On a sender */
			}
		}

		synchronized (message_received) {
			message_received = Boolean.TRUE;
			notify(); /* the sender */
		}
		
		return message;
	}
}
