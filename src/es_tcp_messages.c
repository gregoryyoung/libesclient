#include <stdint.h>
#include "tcp_messages.h"

char *
get_string_for_tcp_message(uint8_t message) {
	switch(message)
	{
		case MESSAGE_HEARTBEATREQUEST : return "Heartbeat Request";
		case MESSAGE_HEARTBEATRESPONSE : return "Heartbeat Response";
		case MESSAGE_PING : return "Ping";
		case MESSAGE_PONG : return "Pong";
		case MESSAGE_PREPAREACK : return "Prepare Acknowledgment";
		case MESSAGE_COMMITACK : return "Commit Acknowledgment";
		case MESSAGE_SLAVEASSIGNMENT : return "Slave Assignment";
		case MESSAGE_CLONEASSIGNMENT : return "Clone Assignment";
		case MESSAGE_SUBSCRIBEREPLICA : return "Subscribe Replica";
		case MESSAGE_REPLICALOGPOSITIONACK : return "Replica Log Position Acknowledgment";
		case MESSAGE_CREATECHUNK : return "Create Chunk";
		case MESSAGE_RAWCHUNKBULK : return "Raw Chunk Bulk";
		case MESSAGE_DATACHUNKBULK : return "Data Chunk Bulk";
		case MESSAGE_REPLICASUBSCRIPTIONRETRY : return "Replica Subscription Retry";
		case MESSAGE_REPLICASUBSCRIBED : return "Replica Subscriber";
		case MESSAGE_WRITEEVENTS : return "Write Events";
		case MESSAGE_WRITEEVENTSCOMPLETED : return "Write Events Completed";
		case MESSAGE_TRANSACTIONSTART : return "Transaction Start";
		case MESSAGE_TRANSACTIONSTARTCOMPLETED : return "Transaction Start Completed";
		case MESSAGE_TRANSACTIONWRITE : return "Transaction Write";
		case MESSAGE_TRANSACTIONWRITECOMPLETED : return "Transaction Write Completed";
		case MESSAGE_TRANSACTIONCOMMIT : return "Transaction Commit";
		case MESSAGE_TRANSACTIONCOMMITCOMPLETED : return "Transaction Commit Completed";
		case MESSAGE_DELETESTREAM : return "Delete Stream";
		case MESSAGE_DELETESTREAMCOMPLETED : return "Delete Stream Completed";
		case MESSAGE_READEVENT : return "Read Event";
		case MESSAGE_READEVENTCOMPLETED : return "Read Event Completed";
		case MESSAGE_READSTREAMEVENTSFORWARD : return "Read Stream Events Forward";
		case MESSAGE_READSTREAMEVENTSFORWARDCOMPLETED : return "Read Stream Events Forward Completed";
		case MESSAGE_READSTREAMEVENTSBACKWARD : return "Read Stream Events Backward";
		case MESSAGE_READSTREAMEVENTSBACKWARDCOMPLETED : return "Read Stream Events Backward Completed";
		case MESSAGE_READALLEVENTSFORWARD : return "Read All Events Forward";
		case MESSAGE_READALLEVENTSFORWARDCOMPLETED : return "Read All Events Forward Completed";
		case MESSAGE_READALLEVENTSBACKWARD : return "Read All Events Backward";
		case MESSAGE_READALLEVENTSBACKWARDCOMPLETED : return "Read All Events Backward Completed";
		case MESSAGE_SUBSCRIBETOSTREAM : return "Subscribe To Stream";
		case MESSAGE_SUBSCRIPTIONCONFIRMATION : return "Subscription Confirmation";
		case MESSAGE_STREAMEVENTAPPEARED : return "Stream Event Appeared";
		case MESSAGE_UNSUBSCRIBEFROMSTREAM : return "Unsubscribe from stream";
		case MESSAGE_SUBSCRIPTIONDROPPED : return "Subscription Dropped";
		case MESSAGE_CONNECTTOPERSISTENTSUBSCRIPTION : return "Connect to Persistent Subscription";
		case MESSAGE_PERSISTENTSUBSCRIPTIONCONFIRMATION : return "Persistent Subscription Confirmation";
		case MESSAGE_PERSISTENTSUBSCRIPTIONSTREAMEVENTAPPEARED : return "Persistent Subscription Stream Event Appeared";
		case MESSAGE_CREATEPERSISTENTSUBSCRIPTION : return "Create Persistent Subscription";
		case MESSAGE_CREATEPERSISTENTSUBSCRIPTIONCOMPLETED : return "Create Persistent Subscription Completed";
		case MESSAGE_DELETEPERSISTENTSUBSCRIPTION : return "Delete Persistent Subscription";
		case MESSAGE_DELETEPERSISTENTSUBSCRIPTIONCOMPLETED : return "Delete Persistent Subscription Completed";
		case MESSAGE_PERSISTENTSUBSCRIPTIONACKEVENTS : return "Persistent Subscription Ack Events";
		case MESSAGE_PERSISTENTSUBSCRIPTIONNAKEVENTS : return "Persistent Subscription Nak Events";
		case MESSAGE_UPDATEPERSISTENTSUBSCRIPTION : return "Update Persistent Subscription";
		case MESSAGE_UPDATEPERSISTENTSUBSCRIPTIONCOMPLETED : return "Update Persistent Subscription Completed";
		case MESSAGE_SCAVENGEDATABASE : return "Scavenge Database";
		case MESSAGE_SCAVENGEDATABASECOMPLETED : return "Scavenge Database Completed";
		case MESSAGE_BADREQUEST : return "Bad Request";
		case MESSAGE_NOTHANDLED : return "Not Handled";
		case MESSAGE_AUTHENTICATE : return "Authenticate";
		case MESSAGE_AUTHENTICATED : return "Authenticated";
		case MESSAGE_NOTAUTHENTICATED : return "Not Authenticated";
		default: return "Unknown Message Code";
	}
}

