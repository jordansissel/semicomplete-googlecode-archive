
import java.util.Scanner;
import java.util.LinkedList;
import java.util.InputMismatchException;
import java.util.NoSuchElementException;
import java.util.Iterator;

public class Pager {

	public PageReplacer p;
	public static Scanner scanner = new Scanner(System.in);

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
		//Scanner s = new Scanner(System.in);
		//System.out.println("RUN!");

		try {
			while (true) {
				int input = scanner.nextInt();
				//System.out.println("Input: " + input);
				p.access(input);
			}
		} catch (InputMismatchException e) {
			System.err.println("Invalid input");
			System.exit(1);
		} catch (NoSuchElementException e) {
			/* End of input, throw this exception out */
		}

		System.out.println("Page faults: " + p.pagefaults);
	}

	private abstract class PageReplacer {
		protected int numframes;
		protected int pagefaults;
		protected LinkedList<Frame> list;

		public PageReplacer(int numframes) {
			this.numframes = numframes;
			pagefaults = 0;
			list = new LinkedList<Frame>();
			for (int x = 0; x < numframes; x++) {
				list.add(new Frame(x));
			}
		}

		public abstract void access(int page);
	}

	private class Frame {
		protected int framenum = -1;
		protected int page = 0;
		protected int counter = 0;
		public Frame(int framenum) { 
			this.framenum = framenum; 
		}

		public String toString() {
			return framenum + ":" + page;
		}
	}

	private class FIFOReplacer extends PageReplacer {
		public FIFOReplacer(int numframes) { 
			super(numframes);
		}

		public void access(int page) {
			/* Take first frame and use it */
			Frame f;
			Iterator<Frame> i = list.iterator();

			//System.out.println("Page: " + page);
			while (i.hasNext()) {
				Frame fr = i.next();
				if (fr.page == page) {
					return;
				}
			}
			
			f = list.remove();
			list.add(f); /* Put it back on the end */
			f.page = page;
			System.out.println(list);

			if (f.counter > 0)
				pagefaults++;

			f.counter++;
		}
	}

	private class LRUReplacer extends PageReplacer {
		public LRUReplacer(int numframes) {
			super(numframes);
		}

		public void access(int page) {
			/* Take first frame and use it */
			Frame f;
			Iterator<Frame> i = list.iterator();

			//System.out.println("Page: " + page);
			while (i.hasNext()) {
				Frame fr = i.next();
				if (fr.page == page) {
					f = list.remove();
					list.add(f); /* Put it back on the end */
					return;
				}
			}
			
			f = list.remove();
			list.add(f); /* Put it back on the end */
			f.page = page;
			System.out.println(list);

			if (f.counter > 0)
				pagefaults++;

			f.counter++;
		}
	}

	public static void main(String args[]) {
		if (args.length != 1) {
			System.err.println("Invalid parameters.");
			System.err.println("Usage: java Pager <lru | fifo>");
			System.exit(1);
		}

		try {
			int numframes = scanner.nextInt();
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
