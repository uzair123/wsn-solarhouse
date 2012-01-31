var motesCount = mote.getSimulation().getMotesCount();
var ttl = 0;

log.log("#--------------\n");
log.log("#gnuplot num_motes: " + motesCount + "\n");
// log.log("#Random seed: " + mote.getSimulation().getRandomSeed() + "\n");

/*
 * Set the test simulated duration (in milliseconds) to some amount of seconds,
 * print last message at timeout
 */
// TIMEOUT(3000000000, log.log("last msg: " + msg + "\n"));
TIMEOUT(30000000, log.log("last msg: " + msg + "\n"));

var epoch = 0;
var members = 0;
var energy_used_epoch = 0;

log.log("#time \n");
log.log("#\t epoch\n");
log.log("#\t\t num results\n");
log.log("#\t\t\t num members\n");
log.log("#\t\t\t\t energy consumed in the epoch\n");


while (epoch < 100) {
	YIELD_THEN_WAIT_UNTIL(msg.contains("bazinga"));

	simTime = mote.getSimulation().getSimulationTime();

	log.log("#" + msg + "\n");
	if (msg.contains("epoch")) {
		epoch = parseInt(msg.substring(msg.indexOf("{") + 1, msg.indexOf("}")));
		results = parseInt(msg
				.substring(msg.indexOf("[") + 1, msg.indexOf("]")));
		log.log(simTime + "\t" + epoch + "\t" + results + "\t - \t - \n");

		// reset energy used in the epoch:
		energy_used_epoch = 0;

		java.lang.System.out.println("Epoch: " + epoch);

	} else if (msg.contains("node_member")) {
		members++;
		log.log(simTime + "\t - \t - \t" + members + "\t - \t #(node: "
				+ mote.getInterfaces().getMoteID().getMoteID() + ")\n");
	} else if (msg.contains("node_not_member")) {
		members--;
		log.log(simTime + "\t - \t - \t" + members + "\t - \t #(node: "
				+ mote.getInterfaces().getMoteID().getMoteID() + ")\n");

	} else if (msg.contains("energy_used")) {
		var energy_used_node = parseInt(msg.substring(msg.indexOf("[") + 1, msg
				.indexOf("]")));
		energy_used_epoch = energy_used_epoch + energy_used_node;
		log.log(simTime + "\t - \t - \t - \t" + energy_used_epoch + "\n");
	}
}
log.log("[" + simTime + "], " + msg + "\n");
log.testOK();
