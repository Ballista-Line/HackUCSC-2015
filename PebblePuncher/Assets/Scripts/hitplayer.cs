using UnityEngine;
using System.Collections;

public class hitplayer : MonoBehaviour {
    public Transform other;
    public Animator animator;

    private float dist;
	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
	    if (other)
        {
            dist = Vector3.Distance(other.position, transform.position);
        }
        if (dist <= 2.4)
        {
            animator.SetTrigger("Swing");
        }
	}
}
