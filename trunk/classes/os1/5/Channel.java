

/* This is an object we can instantiate, and send() and receive() are non-static methods.
 * therefore there can be more than one independent channel */
public class Channel<E> {
	private E message;

	private Object sent = new Object(), received = new Object();
	private Boolean message_sent = new Boolean(false);
	private Boolean message_received = new Boolean(false);

	/* These methods are synchronized, therefore only one thread can access each
	 * method at a time */
	public void send(E message) {
		synchronized (sent) {

			message_sent = Boolean.FALSE;
			/* Notify a receiver, if any, that we have sent a message */
			synchronized (this) { 
				/* "Send" the message */
				this.message = message;

				/* Specify that our message has been sent */
				message_sent = Boolean.TRUE; /* Needed if no receiver yet */
				notify();  /* Wake up the receiver if there is one */

				/* If the message has not been received yet, then indicate the message has been sent
				 * and wait for a notification of reception */
				System.out.println(Thread.currentThread().getId() + " waiting on receiver");
				while (! message_received.booleanValue()) {
					try {
						this.wait(); /* On a receiver */
						System.out.println(Thread.currentThread().getId() + " wakeup - message_received = " + message_received);
					} catch (Exception e) { }
				}
				System.out.println(Thread.currentThread().getId() + " receiver notified me");
				notify();
			}

		} 
	}

	/* This method is also synchronized, so only one thread can receive at a time */
	public E receive() {
		synchronized (received) {
			message_received = Boolean.FALSE;
			System.out.println(Thread.currentThread().getId() + " ENTER");

			synchronized (this) {
				/* If no message was sent wait for a message to come in */
				if (! message_sent.booleanValue()) {

					/* Wait until a message arrives */
					try {
						wait(); /* On a sender */
					} catch (Exception e) { }
				}

				/* Sender will not wait if message_received is true */
				System.out.println(Thread.currentThread().getId() + " Setting received to true");
				message_received = Boolean.TRUE;
				System.out.println(Thread.currentThread().getId() + " THIS VALUE IS TRUE: " + message_received);

				/* Wake up sender */
				notify(); /* the sender */

				/* acknowledge the recept */
				try {
					this.wait(); /* On a receiver */
					System.out.println(Thread.currentThread().getId() + " waiting on sender");
				} catch (Exception e) { }
			}

			System.out.println(Thread.currentThread().getId() + " LEAVE");
			return message;
		}
	}


	public static void main(String[] args) {
		final Channel channel = new Channel();

		for(int i = 0; i < 4; i++) {
			new Thread(new Runnable() {
				public void run() {
					//System.out.println(Thread.currentThread().getId() + " sending...");
					channel.send("hi");
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
