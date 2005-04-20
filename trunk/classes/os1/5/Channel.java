

/* This is an object we can instantiate, and send() and receive() are non-static methods.
 * therefore there can be more than one independent channel */
public class Channel<E> {

	/* Message can be null because it's value is nothing special */
	private E message;

	/* Locker objects */
	private Object sent = new Object(), received = new Object();

	/* State */
	private boolean message_sent = false;
	private boolean message_received = false;

	/* These methods are synchronized, therefore only one thread can access each
	 * method at a time */
	public void send(E message) {
		synchronized (sent) { /* sync on sent to limit to one thread */
			synchronized (this) { /* sync on this so we can wait() and notify() on it */
				this.message = message; /* "Send" the message */

				/* Specify that our message has been sent */
				message_sent = true; /* Needed if no receiver yet */
				notify();  /* Wake up the receiver if there is one */

				/* If the message has not been received yet, then indicate the message has been sent
				 * and wait for a notification of reception */
				while (!message_received) {
					try {
						wait(); /* On a receiver */
					} catch (Exception e) { }
				}
				message_received = false;
			}
		} 
	}

	/* This method is also synchronized, so only one thread can receive at a time */
	public E receive() {
		synchronized (received) { /* Synchronize on received so only one thread can access us */
			synchronized (this) { /* Synchronize on this so we can lock on it */
				E mymessage;

				/* If no message was sent wait for a message to come in */
				while (!message_sent) {
					/* Wait until a message arrives */
					try {
						wait(); /* On a sender */
					} catch (Exception e) { }
				}

				/* Sender will not wait if message_received is true */
				mymessage = message;
				message_received = true;
				message_sent = false;
				notify(); /* the sender */
				return mymessage;
			}
		}
	}


	/* We'll pretend that this isn't test code */

	/* 
	 *
	 *
	 * no really.. there is no test code here.
	 *
	 *
	 */
	public static void main(String[] args) {
		final Channel<Integer> channel = new Channel<Integer>();

		for(int i = 0; i < 300; i++) {
			final int x = i; /* anonymous classes like final */
			new Thread(new Runnable() {
				public void run() {
					//System.out.println(Thread.currentThread().getId() + " sending: " + MESSAGE);
					channel.send(new Integer(x));
					//System.out.println(Thread.currentThread().getId() + " sending done");
				}
			}).start();

			new Thread(new Runnable() {
				public void run() {
					//System.out.println(Thread.currentThread().getId() + " receiving...");
					System.out.println(channel.receive());
					//System.out.println(Thread.currentThread().getId() + " receiving done");
				}
			}).start();
		}
	}

}
