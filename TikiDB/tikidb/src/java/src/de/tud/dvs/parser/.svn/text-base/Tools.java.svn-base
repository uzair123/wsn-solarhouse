package de.tud.dvs.parser;


public class Tools {

	private static int debugLevel = 4;

	/**
	 * Converts a Java integer (i.e. a 32-bit signed two's complement integer
	 * whose value ranges from -2,147,483,648 to 2,147,483,647) which should
	 * contain a value between [0..255] into a Java byte (i.e. an 8-bit signed
	 * two's complement integer whose value ranges from -128 to 127). If the
	 * Java integer's value passed as parameter is not in the expected range
	 * (i.e. [0..255], an <code>IntegerValueException<code> is thrown.
	 * 
	 * @param unsigned
	 *            the unsigned <code>int</code> to be converted
	 * @return
	 * @throws IntegerValueException
	 */
	public static byte sign(int unsigned) throws IllegalArgumentException {

		if ((unsigned < 0) || (unsigned > 255))
			throw new IllegalArgumentException(
					"Integer passed as parameter is not in the range [0..255]");
		else {
			byte result;

			if (unsigned > 127) {
				unsigned -= 256;
			}

			result = (byte) unsigned;
			return result;
		}
	}

	/**
	 * Converts a Java byte (i.e. an 8-bit signed two's complement integer whose
	 * value ranges from -128 to 127) into a Java integer (i.e. a 32-bit signed
	 * two's complement integer whose value ranges from -2,147,483,648 to
	 * 2,147,483,647 but will range from [0..255])
	 * 
	 * @param sign
	 *            the <code>byte</code> to be converted
	 * @return
	 */
	public static int unsign(byte sign) {
		int result = sign;

		if (result < 0) {
			result += 256;
		}
		return result;
	}

	/**
	 * Changes the debugging level: <br>
	 * 0 = no debugging, 1, 2, 3, 4, 5 = max debugging
	 * 
	 * @return
	 */
	public static int debugLevel() {
		return debugLevel;
	}

	public static void setDebugLevel(int level) {
		if ((level >= 0) && (level <= 5))
			debugLevel = level;
	}

	/**
	 * Given a string, returns a 16 bit integer
	 * 
	 * @param name
	 *            the String to hash
	 * @return the resulting hashed value
	 */
	public static int hashName(String name) {
		int scopeId = name.hashCode();

		if (scopeId < 0)
			scopeId *= -1;
		scopeId = scopeId % 65536;
		if (scopeId == 0)
			scopeId = 1;

		return scopeId;
	}

}
