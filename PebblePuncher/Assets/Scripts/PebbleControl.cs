using UnityEngine;
using System.Collections;

public class PebbleControl : MonoBehaviour
{
    public string url = "http://www.faustfamily.me/?id=004441df9afa72984c974fab35774222";
    private float timeElapsed;
    private float respawnEnemy;
    private WWW www;
    private bool enemyDead;
    public Animator animator;
    public GameObject enemy;
    void Start()
    {
        timeElapsed = 0.0f;
        respawnEnemy = 0.0f;
        enemyDead = false;
    }

    void Update()
    {
        if (timeElapsed >= 0.2f)
        {
            StartCoroutine(swingSwordFromPebble(www));
            timeElapsed = 0.0f;
            
        }
        bool swingSword = Input.GetButtonDown("Fire1");
        if (swingSword)
        {
            animator.SetTrigger("Swing");
        }

        timeElapsed += Time.deltaTime;
        if (enemyDead)
        {
            if (respawnEnemy >= 5.0f)
            {
                enemy.gameObject.SetActive(true);
                respawnEnemy = 0.0f;
            }
            respawnEnemy += Time.deltaTime;
        }
    }
    IEnumerator swingSwordFromPebble(WWW www)
    {
        www = new WWW(url);
        yield return www;
        JSONObject pebbleInput = new JSONObject(www.text);

        //Debug.Log(pebbleInput.GetField("ACTION").n);

        int swingSword = (int)pebbleInput.GetField("ACTION").n;
        if (swingSword == 0 || swingSword == 1)
        {
            animator.SetTrigger("Swing");
            swingSword = 1;
            www = new WWW(url + "&over={\"MAGA\":0,\"ACTION\":-1}");
        }

        if (www.error != null)
        {
            Debug.Log("WWW Error! " + www.error);
        }
    }
    void OnTriggerEnter(Collider other)
    {
        if (other.gameObject.CompareTag("Enemy"))
        {
            other.gameObject.SetActive(false);
            enemyDead = true;
        }
    }

}
