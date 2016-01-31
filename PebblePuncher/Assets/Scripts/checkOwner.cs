using UnityEngine;
using System.Collections;
using UnityEngine.Networking;

public class checkOwner : NetworkBehaviour
{
    public GameObject cam;
    // Use this for initialization
    void Start()
    {
        if (!isLocalPlayer)
        {
            cam.gameObject.SetActive(false);
        }
    }
}