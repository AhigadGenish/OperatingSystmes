using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

// producer class and unbounded and bounded queues class.
namespace ProducerConsumer
{

    // bounded queue

    internal class BQ
    {
        // data members
        public int BoundedSize;
        public int CurSize;
        public List<string> Bq;
        public Mutex mutex; // mutual exclusion 
        public Semaphore empty; // count of empty buffer slots
        public Semaphore full; // count of full buffer slots
        public void Enqueue(string x)
        {
            // semapohore
            this.empty.WaitOne();
            // lock
            this.mutex.WaitOne();
            // enqueue
            this.Bq.Add(x);
            // unlock
            this.mutex.ReleaseMutex(); // leave critical section
            // semaphore
            this.full.Release(); // one more full buffer
            this.CurSize++;

        }
        public string Dequeue()
        {
            // semapohore:
            this.full.WaitOne();
            // lock
            this.mutex.WaitOne();
            // dequeue
            string first = this.Bq[0];
            this.Bq.RemoveAt(0);
            // unlock
            this.mutex.ReleaseMutex(); // leave critical section
            // sempaphore
            this.empty.Release(); // one more full buffer
            this.CurSize--;
            return first;
        }
        // constructor

        public BQ(int boundedSize)
        {
            this.Bq = new List<string>();
            this.BoundedSize = boundedSize;
            this.CurSize = 0;
            this.mutex = new Mutex(); ; // mutual exclusion 
            this.empty = new Semaphore(BoundedSize, BoundedSize); // count of empty buffer slots
            this.full = new Semaphore(0, BoundedSize); // count of full buffer slots
        }
    }
    // unbounded Queue
    internal class UBQ
    {
        // data members
        public int CurSize;
        public List<string> UQ;
        public Mutex mutex; // mutual exclusion 
        public Semaphore full; // count of full buffer slots

        public void Enqueue(string x)
        {
            // mutex:
            this.mutex.WaitOne();
            // enqueue
            this.UQ.Add(x);
            // unlock
            this.mutex.ReleaseMutex(); // leave critical section
            // semaphore
            this.full.Release();
            this.CurSize += 1;
        }
        public string Dequeue()
        {   // semapohore:
            this.full.WaitOne();
            // lock
            this.mutex.WaitOne();
            // dequeue
            string first = this.UQ[0];
            this.UQ.RemoveAt(0);
            // unlock
            this.mutex.ReleaseMutex(); // leave critical section
            return first;
        }
        // constructor
        public UBQ()
        {
            this.UQ = new List<string>();
            this.CurSize = 0;
            this.mutex = new Mutex(); // mutual exclusion 
            this.full = new Semaphore(0, int.MaxValue); // count of full buffer slots
        }

    }

    internal class Producer
    {
        // data members
        public int Id;
        public BQ Bq;
        public int NumOfString;
        // coonstructor
        public Producer(int id, int Qsize, int numOfString)
        {
            this.Id = id;
            this.Bq = new BQ(Qsize);
            this.NumOfString = numOfString;
        }
    }
}

   


