import json
import boto3

def hello(event, context):
    key=event['params']['querystring']['key']
    dynamodb.get_item(TableName="digitar", Key='user':{'S':key}})
    body = {
        "message": "Go Serverless v1.0! Your function executed successfully!",
        "input": event
    }

    response = {
        "statusCode": 200,
        "body": json.dumps(body)
    }

    return response

    # Use this code if you don't use the http event with the LAMBDA-PROXY
    # integration
    """
    return {
        "message": "Go Serverless v1.0! Your function executed successfully!",
        "event": event
    }
    """

def put(event, context):
    key=event['params']['querystring']['key']
    value=event['params']['querystring']['value']
    dynamodb.put_item(TableName="digitar", Item={'user':{'S':key},
        'issues':{'S':value}}})
    body = {
        "message": "Go Serverless v1.0! Your function executed successfully!",
        "input": event
    }

    response = {
        "statusCode": 200,
        "body": json.dumps(body)
    }

    return response

    # Use this code if you don't use the http event with the LAMBDA-PROXY
    # integration
    """
    return {
        "message": "Go Serverless v1.0! Your function executed successfully!",
        "event": event
    }
    """
