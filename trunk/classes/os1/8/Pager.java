
import java.util.Scanner;
import java.util.LinkedList;
import java.util.InputMismatchException;
import java.util.NoSuchElementException;

public class Pager {

	PageReplacer p;

	private class InvalidPagerTypeException extends Exception { 
		public InvalidPagerTypeException() { super(); }
	}

	public Pager(String type, int numframes) throws InvalidPagerTypeException { 
		if (type.equals("fifo")) {
			p = new FIFOReplacer(numframes);
		} else if (type.equals("lru")) {
			p = new LRUReplacer(numframes);
		} else {
			throw new InvalidPagerTypeException();
		}
	}

	public void run() {
		Scanner s = new Scanner(System.in);

		try {
			while (s.hasNextInt()) {
				int input = s.nextInt();
				p.access(input);
			}
		} 

		System.out.println("
	}

	private abstract class PageReplacer {
		protected int numframes;
		protected int pagefaults;
		public abstract void access(int page);
	}

	private class Frame {
		protected int page = 0;
		protected int counter = 0;
	}

	private class FIFOReplacer extends PageReplacer {
		protected LinkedList<Frame> fifo;
		public FIFOReplacer(int numframes) { 
			this.numframes = numframes;
			pagefaults = 0;
			fifo = new LinkedList<Frame>();
			for (int x = 0; x < numframes; x++) {
				fifo.add(new Frame());
			}
		}

		public void access(int page) {
			/* Take first frame and use it */
			Frame f;
			Iterator i = fifo.iterator();

			while (i.hasNext()) {
				Frame fr = i.next();
				if (fr.page = page) {
					return;
				}
			}
			
			f = fifo.remove();
			fifo.add(f); /* Put it back on the end */

			if (f.counter)
				pagefaults++;

			f.counter++
		}
	}

	private class LRUReplacer extends PageReplacer {
		public void access(int page) {

		}
	}

	public static void main(String args[]) {
		Scanner s = new Scanner(System.in);
		if (args.length != 1) {
			System.err.println("Invalid parameters.");
			System.err.println("Usage: java Pager <lru | fifo>");
			System.exit(1);
		}

		try {
			int numframes = s.nextInt();
			Pager pg = new Pager(args[0], numframes);
			pg.run();
		} catch (InvalidPagerTypeException e) {
			System.err.println("Invalid pager type: " + args[0]);
			System.exit(1);
		} catch (InputMismatchException e) {
			System.err.println("Invalid input");
			System.exit(1);
		} catch (NoSuchElementException e) {
			System.err.println("No input received");
		}
	}

} /* Pager */
