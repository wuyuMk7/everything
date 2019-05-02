'use strict';

const crypto = require('crypto');
const functions = require('firebase-functions');
const { DateTime } = require('luxon');

const firebase = require('firebase');
require('firebase/auth');
require('firebase/firestore');
const sittingloggerConfig = {
  apiKey: "<APIKey>",
  authDomain: "test-auth-55c6f.firebaseapp.com",
  databaseURL: "https://test-auth-55c6f.firebaseio.com",
  projectId: "test-auth-55c6f",
  storageBucket: "test-auth-55c6f.appspot.com",
  messagingSenderId: "<MessageSenderID>"
};
const sittingloggerApp = firebase.initializeApp(sittingloggerConfig, "sittinglogger");
const sittingloggerDb = sittingloggerApp.firestore();

exports.report = functions.https.onRequest(async (request, response) => {
  let err_resp = { "status": "error", "msg": "" }

  const data = request.body;
  //console.log(data);
  if(data.email == undefined || data.email == "") {
    err_resp['msg'] = "Invalid email.";
    response.send(err_resp);
    return
  }
  if(data.datetime == undefined || data.datetime == "") {
    err_resp['msg'] = "Invalid date format.";
    response.send(err_resp);
    return
  }
  if(data.isSitting != 0 && data.isSitting != 1) {
    err_resp['msg'] = "Invalid sitting data.";
    response.send(err_resp);
    return
  }
  if(data.sign == undefined || data.sign == "") {
    err_resp['msg'] = "Authentication failed.";
    response.send(err_resp);
    return
  }

  const datetimeObject = DateTime.fromISO(data.datetime, { setZone: true }) 
  const docName = datetimeObject.toFormat('yyyy-MM-dd');
  const attrName = datetimeObject.hour * 60 + datetimeObject.minute;

  try {
    let userDoc = await sittingloggerDb.collection(data.email).doc('profile').get();
    let error = false;
    if (!userDoc.exists) {
      error = true;
    } else {
      try {
        let key = Buffer.from(userDoc.data().key, 'base64');
        let sign = Buffer.from(data.sign, 'base64');
        let iv = sign.slice(0, 16);
        let decipher = crypto.createDecipheriv('aes-256-cfb', key, iv);
        let plaintext = decipher.update(sign.slice(16)) + decipher.final()

	let expect_str1 = "", expect_str2 = "";
	let expect_time = datetimeObject.toFormat('yyyyMMddHHmm');
	expect_str1 = data.email + expect_time;
	if (userDoc.data().gmail != undefined && userDoc.data().gmail != "")
	  expect_str2 = userDoc.data().gmail + expect_time;
		   
	if (plaintext != expect_str1 && plaintext != expect_str2)
	  error = true;
      } catch(err) {
        console.log(err);
	error = true;
      }
    }

    if (error) {
      err_resp['msg'] = "Authentication failed.";
      response.send(err_resp);
      return
    }
  } catch (err) {
    console.log(err) 
  }

  let docData = {};
  docData[attrName] = data.isSitting;

  sittingloggerDb
	.collection(data.email)
	.doc('sitting-signal')
	.collection('raw_data')
	.doc(docName)
	.set(docData, { merge: true });

  response.send({ "status": "success" })
});
