package de.tud.dvs.contiki;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;

public class SerialConn {
	public static final String SERIALDUMP_LINUX = "/Users/guerrero/workspace/contiki-2/tools/sky/serialdump-linux";

	private Process serialDumpProcess;

	private String comPort;

	private BufferedReader input;
	private Thread readInput;

	private BufferedWriter output;
	private Thread readErrInput;

	private BufferedReader err;

	public SerialConn(String comPort) {
		this.comPort = comPort;
		System.out.println("Trying to connect to COM port: " + this.comPort);

		/* Connect to COM using external serialdump application */
		String fullCommand;
		fullCommand = SERIALDUMP_LINUX + " " + "-b115200" + " " + comPort;

		try {
			String[] cmd = fullCommand.split(" ");
			serialDumpProcess = Runtime.getRuntime().exec(cmd);
			this.input = new BufferedReader(new InputStreamReader(
					serialDumpProcess.getInputStream()));
			this.output = new BufferedWriter(new OutputStreamWriter(
					serialDumpProcess.getOutputStream()));
			this.err = new BufferedReader(new InputStreamReader(
					serialDumpProcess.getErrorStream()));

			/* Start thread listening on stdout */
			Thread readInput = new Thread(new Runnable() {
				public void run() {
					String line;
					try {
						while ((line = input.readLine()) != null) {
							parseIncomingLine(line);
						}
						input.close();
						System.out
								.println("Serialdump process shut down, exiting");
						System.exit(1);
					} catch (IOException e) {
						System.err
								.println("Exception when reading from serialdump");
						e.printStackTrace();
						System.exit(1);
					}
				}
			}, "read input stream thread");

			/* Start thread listening on stdout */
			Thread readErrInput = new Thread(new Runnable() {
				public void run() {
					String line;
					try {
						while ((line = err.readLine()) != null) {
							parseIncomingLine(line);
						}
						err.close();
						System.out
								.println("Serialdump process shut down, exiting");
						System.exit(1);
					} catch (IOException e) {
						System.err
								.println("Exception when reading from serialdump");
						e.printStackTrace();
						System.exit(1);
					}
				}
			}, "read input error stream thread");

			readInput.start();
			readErrInput.start();

		} catch (Exception e) {
			System.err
					.println("Exception when executing '" + fullCommand + "'");
			System.err.println("Exiting demo application");
			e.printStackTrace();
			System.exit(1);
		}
	}

	public void send(String data) {
		if (output != null) {
			try {
			output.write(data.toCharArray());
			output.flush();
			}
			catch (IOException iox) {
				iox.printStackTrace();
			}
		}
	}

	public void parseIncomingLine(String line) {
		if (line == null) {
			System.err.println("Parsing null line");
			return;
		}

		/* Split line into components */
		String[] components = line.split(" ");

		for (int i = 0; i < components.length; i++) {
			System.out.print(components[i] + " ");
		}
		System.out.println();

	}

	public static void main(final String[] args) throws Exception {
		if (args.length != 1) {
			System.err.println("Usage: java SerialConn COMPORT");
			return;
		}

		final String comPort = args[0];
		SerialConn conn = new SerialConn(comPort);
		
		Thread.sleep(4000);
		
		System.out.println("Will send 'info'");
		conn.send("info\n");
		
		
	}
}
