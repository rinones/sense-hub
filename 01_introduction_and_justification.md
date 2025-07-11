# DIY Environmental and Security Monitoring System with Mainframe Integration (COBOL, CICS, DB2)

## Introduction & Project Justification

In the world of critical systems powering banking, stock exchanges, and major financial institutions, robustness, reliability, and high-concurrency processing are not just technical requirements—they are the foundation for trust and operational continuity. This project showcases advanced competencies in mainframe environments—specifically COBOL, CICS, and DB2—by developing a DIY environmental and security monitoring solution that faithfully replicates the challenges and standards found in real-world production systems.

## Emulating the Standards of Critical Financial Systems

The proposed architecture draws inspiration from the best practices and standards that govern transactional, high-availability, and secure systems. Each event or sensor reading—collected by a network of ESP32-based microcontrollers and a variety of environmental and security modules—is transmitted, processed, and stored in a simulated mainframe backend. The core processing, business logic, event management, persistence, and auditing are handled in COBOL, orchestrated under CICS, and stored in DB2, mirroring the operational rigor of financial institutions.

This design addresses the same challenges as those in banking and finance:
- **High concurrency:** Capability to receive and process multiple events in parallel, mimicking the constant flow of banking operations.
- **Reliability & fault tolerance:** Redundancy and functional segmentation are simulated via multiple microcontrollers and nodes.
- **Traceability & auditing:** Detailed logging of every event, enabling complete reconstruction of any transaction or alert's lifecycle.
- **Scalability:** Modular architecture allows the addition of new sensors or nodes, as in large-scale IT infrastructures.
- **Security:** Data transmission uses robust protocols, with access control at every system layer.

## Integration: Microcontrollers, Sensors, and the Mainframe Backend

The dataflow begins with sensors connected to ESP32 microcontrollers, capturing environmental variables (temperature, humidity, air quality, motion detection, etc.) and security events. These readings are transmitted using industry-standard protocols (such as HTTP(S) or MQTT) to a middleware gateway, which validates, traces, and transforms the messages into mainframe-compatible transactions. On the backend, COBOL programs under CICS process and store these events in DB2, generating audit records and enabling real-time analysis and querying—just like in major financial organizations.

## Learning Potential & Demonstrated Skills

This project serves as an outstanding showcase for any IT professional aspiring to mainframe roles or positions in banking and mission-critical enterprise environments. It demonstrates not only proficiency in electronics and microcontroller programming, but also in designing and managing robust, secure, and scalable architectures that align with the high standards of the financial industry. Using multiple ESP32 modules to simulate scenarios of scalability, redundancy, and fault tolerance brings the project even closer to modern enterprise architectures, covering the entire journey from edge data acquisition to transactional management and auditing in the banking core.

Ultimately, this initiative reflects the ability to design, implement, and document complex solutions focused on operational excellence and information security, providing a clear competitive advantage for any IT portfolio targeting roles in finance or mission-critical systems.