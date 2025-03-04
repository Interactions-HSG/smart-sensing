package interactions.ics.unisg.ch.smartsensing;


import interactions.ics.unisg.ch.smartsensing.entities.GroupRole;
import org.apache.jena.query.*;
import org.apache.jena.rdf.model.Literal;
import org.apache.jena.rdf.model.RDFNode;
import org.apache.jena.rdf.model.Resource;
import org.apache.jena.rdfconnection.RDFConnection;
import org.apache.jena.system.Txn;

import java.util.ArrayList;

public class RDFTripleStore {

    private Dataset dataset;
    RDFConnection connection;
    public void init(){
        Query query = QueryFactory.create("SELECT * {?s ?p ?o }");
        dataset = DatasetFactory.createTxnMem();
        RDFConnection connection = RDFConnection.connect(dataset);

        Txn.executeWrite(connection, () ->{
            System.out.println("Loading base ontologies");
            connection.load("oshes.owl.ttl");
            //connection.load("http://example/g0", "data.ttl");
            System.out.println("In write transaction");
            connection.queryResultSet(query, ResultSetFormatter::out);
        });
        // And again - implicit READ transaction.
        System.out.println("After write transaction");
        connection.queryResultSet(query, ResultSetFormatter::out);
    }

    public ArrayList<String> getRoleIds(){
        ArrayList<String> roles = new ArrayList<>();
        Query query = QueryFactory.create("SELECT ?role {?role a osh:Role }");
        QueryExecution qExec = QueryExecutionFactory.create(query, dataset);
        ResultSet results = qExec.execSelect() ;
        for ( ; results.hasNext() ; )
        {
            QuerySolution soln = results.nextSolution() ;
            RDFNode x = soln.get("role") ;       // Get a result variable by name.
            roles.add(x.toString());
        }
    }

    public void addRole(GroupRole role){
        String id = role.getURI();
        Query query = QueryFactory.create(String.format("INSERT {<%s> a osh:Role }", id));
        QueryExecution qExec = QueryExecutionFactory.create(query, dataset);
        ResultSet results = qExec.execSelect() ;
    }
}
