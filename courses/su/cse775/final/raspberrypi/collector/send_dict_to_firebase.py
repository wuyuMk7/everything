import firebase_admin
from firebase_admin import credentials
from firebase_admin import firestore
import config as cfg
import time
from datetime import datetime
from tzlocal import get_localzone
# from pathlib import Path

def createDb():
    # firebase_path=Path.cwd()/cfg.FIREBASE_CREDS_JSON
    cred=credentials.Certificate(cfg.FIREBASE_CREDS_JSON)
    firebase_admin.initialize_app(cred)
    db=firestore.client()
    return db

def sendToFirebase(firebase_values,collectionName=cfg.FIREBASE_COLLECTION_NAME,documentName=cfg.FIREBASE_DOCUMENT_NAME,subCollection=cfg.FIREBASE_SITTING_SUBCOLLECTION_NAME):
    try:
        db=createDb()
        #db.collection(collectionName).document(documentName).set(firebase_values)
        #
        ##add server timestamps to specific fields in your documents
        #db.collection(collectionName).document(documentName).update({
        #    'timestamp':firestore.SERVER_TIMESTAMP
        #})

        current_datetime = firebase_values['datetime']
        doc_name = current_datetime.strftime('%Y-%m-%d')
        attr_name = str(current_datetime.hour * 60 + current_datetime.minute)

        #db.collection(collectionName).document(documentName).collection(subCollection).document(doc_name).set({ attr_name: firebase_values['isSitting']}, merge=True)
        #db.collection("xzhan145@syr.edu").document(documentName).collection(subCollection).document(doc_name).set({ attr_name: firebase_values['isSitting']}, merge=True)
        #db.collection("yhe106@syr.edu").document(documentName).collection(subCollection).document(doc_name).set({ attr_name: firebase_values['isSitting']}, merge=True)

    except Exception as e:
        print(e)
        print('error')

if __name__ == "__main__":
    # Can also use datetime.now() to get current timestamp
    current_time = time.time()
    firebase_values={
        'isSitting':1,
        'datetime': datetime.fromtimestamp(current_time, get_localzone()),
        'timestamp': int(current_time)
    }
    # no need to pass time into the dictionary value that will be sent to the fb
    sendToFirebase(firebase_values)

