
import java.util.*;
import java.util.concurrent.ConcurrentLinkedQueue;

public class SleepingBarber {

	private Scanner scan;

	public SleepingBarber() {
		scan = new Scanner(System.in);
	}

	public void start() {
		int numchairs = 0;
		try { numchairs = scan.nextInt(); } 
		catch (Exception e) { System.err.println(e); }


		BarberShop bs = new BarberShop(numchairs);
		Barber b = new Barber(bs);
		b.setName("barber");

		bs.print("creates " + numchairs + " chairs");

		b.start();

		try {
			int cnt = 1;
			int n;
			while (scan.hasNextInt()) {
				Customer c;
				String name;
				n = scan.nextInt();
				//bs.print("sleeping for " + n + " seconds");
				Thread.currentThread().sleep(n * 1000);
				name = "beard " + cnt;
				bs.print("sends " + name + " to shop");
				c = new Customer(bs);
				c.setName(name);
				c.start();
				cnt++;
			}
		} catch (Exception e) {
			System.err.println(e);
		}

		bs.print("tells barber to quit");
		bs.print("ends: no more customers");
		b.setFinished(true);

	}

	public static void main(String args[]) throws InputMismatchException, NoSuchElementException, IllegalStateException {
		SleepingBarber sb = new SleepingBarber();

		sb.start();
	}


	private class BarberShop {
		protected int numchairs;
		protected ConcurrentLinkedQueue<Customer> chairs;

		public BarberShop(int numchairs) {
			this.numchairs = numchairs;
			chairs = new ConcurrentLinkedQueue<Customer>();
		}

		protected final Calendar cal = Calendar.getInstance();
		protected final long startTime = new Date().getTime();
		public synchronized void print (String message) {
			cal.setTimeInMillis(new Date().getTime() - startTime);
			System.out.format("%tM:%tS %s %s\n",
									cal, cal, Thread.currentThread().getName(), message);
		}

		public synchronized int numChairsEmpty() {
			return numchairs - chairs.size();
		}

		public synchronized boolean hasEmptySeat() {
			return (chairs.size() < numchairs);
		}

		public synchronized void addCustomer(Customer c) /*throws FullBarberShopException */{
			if (chairs.size() < numchairs) {
				chairs.add(c);
			} else {
				//throw new FullBarberShopException();
			}
		}

		public synchronized Customer getNextCustomer() throws EmptyBarberShopException {
			Customer c = null;
			try {
				c = chairs.remove();
			} catch (NoSuchElementException e) {
				throw new EmptyBarberShopException();
			}

			return c;
		}
	}

	private class EmptyBarberShopException extends Exception {
		public EmptyBarberShopException() {
			super();
		}
	}

	private class FullBarberShopException extends Exception {
		public FullBarberShopException() {
			super();
		}
	}

	protected class Customer extends Thread {
		private BarberShop shop;
		private boolean beingshaved = false;
		private boolean done = false;

		public Customer(BarberShop bs) {
			shop = bs;
		}

		public synchronized void startShaving() {
			beingshaved = true;
			notify(); /* Wake up myself, i'm being shaved */
		}

		public synchronized void finishShaving() {
			done = true;
			notify(); /* Wake up myself, I'm done being shaved */
		}

		public synchronized void run() {
			synchronized (shop) {
				if (shop.hasEmptySeat()) {
					shop.addCustomer(this);
				} else {
					shop.print("leaves shop");
					return;
				}
				shop.notify();
			}

			/* Wait until it's our turn to be shaved */
			try {
				shop.print("starts waiting");
				while (!beingshaved)
					wait(); /* until my turn to get shaved! */
				shop.print("gets shaved");

				while (!done)
					wait(); /* until the barber is done shaving me */
				shop.print("is done");
			} catch (InterruptedException e) {
				System.err.println(e);
			}


		}

	}

	protected class Barber extends Thread {
		private BarberShop shop;
		private boolean done = false;

		public Barber(BarberShop bs) {
			shop = bs;
		}

		public setFinished(boolean f) {
			done = f;
		}

		public void run() {
			while (!done) {
				Customer c = null;
				while (null == c) {
					try {
						c = shop.getNextCustomer();
					} catch (EmptyBarberShopException e) {
						shop.print("sleeps");
						synchronized (shop) { 
							try {
								shop.wait(); 
							} catch (InterruptedException ie) {
								System.err.println(e);
							}
						}
					}
				}

				/* Now we have a valid customer, Shave him! */
				//shop.print("--> shaving " + c);
				c.startShaving();
				try {
					sleep(5000);
				} catch (InterruptedException e) {
					System.err.println(e);
				}

				/* Finish shaving the customer */
				//shop.print("--> done with " + c);
				c.finishShaving();
			}
			shop.print("quits");
		}

	}

}
