

/* This is an object we can instantiate, and send() and receive() are non-static methods.
 * therefore there can be more than one independent channel */
public class Channel<E> {
	E message;

	Boolean message_sent;
	Boolean message_received;

	/* These methods are synchronized, therefore only one thread can access each
	 * method at a time */
	public void send(E message) {
		synchronized (message_sent) {

			/* Notify a receiver, if any, that we have sent a message */
			synchronized (this) { 
				/* "Send" the message */
				this.message = message;

				/* Specify that our message has been sent */
				message_sent = Boolean.TRUE; /* Needed if no receiver yet */
				this.notify();  /* Wake up the receiver if there is one */

				/* If the message has not been received yet, then indicate the message has been sent
				 * and wait for a notification of reception */
				while (! message_received.booleanValue()) {
					try {
						this.wait(); /* On a receiver */
					} catch (Exception e) { }
				}
			}

			message_sent = Boolean.FALSE;
		} 
	}

	/* This method is also synchronized, so only one thread can receive at a time */
	public E receive() {
		//synchronized (message_received) {

			synchronized (this) {
				/* If no message was sent wait for a message to come in */
				if (! message_sent.booleanValue()) {

					/* Wait until a message arrives */
					try {
						wait(); /* On a sender */
					} catch (Exception e) { }
				}

				/* Sender will not wait if message_received is true */
				message_received = Boolean.TRUE;

				/* Wake up sender */
				notify(); /* the sender */
			}

			message_received = Boolean.FALSE;
			return message;
		//}
	}
}
