import java.util.*;
import java.math.BigInteger;

public class fib {
	public static Object toobig_mutex = new Object();
	public static void main(String args[]) {
		List input = Collections.synchronizedList(new LinkedList());

		fibcalc calcthread = new fibcalc();
		calcthread.setQueue(input);

		calcthread.start();

		for (;;) {

			synchronized(toobig_mutex) {
				if (input.size() < 10)
					toobig_mutex.notify();
			}

			if (input.size() > 5) {
				synchronized (input) {
					// pop off 5 results
					for (int i = 0; i < 5; i++)  {
						System.out.println(input.remove(0));
						//try { Thread.sleep(500); }
						//catch (Exception e) { }
					}
				}
			}
			Thread.currentThread().yield();
		}
	}

	private static class fibcalc extends Thread {
		private List output;

		public void setQueue(List l) {
			output = l;
		}

		public void run() {
			BigInteger x, y, result;
			x = new BigInteger("1");
			y = new BigInteger("1");
			result = new BigInteger("0");

			output.add(x);
			output.add(y);

			for (;;) {
				synchronized(toobig_mutex) {
					if (output.size() > 10) {
						try {
							System.out.println("fibcalc waiting, queue too far ahead");
							toobig_mutex.wait();
						} catch (Exception e) { }
					}
				}

				result = x.add(y);
				y = x;
				x = result;

				//System.out.println("fibcalc: " + result);
				output.add(result);
			}
		}

	}
}
